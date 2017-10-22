/*********************************************************************************
**
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "CFDcaseInstance.h"
#include "CFDanalysisType.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/filetreenode.h"
#include "../AgaveExplorer/remoteFileOps/easyboollock.h"
#include "../AgaveExplorer/remoteFileOps/joboperator.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/remotejobdata.h"

#include "vwtinterfacedriver.h"

CFDcaseInstance::CFDcaseInstance(FileTreeNode * newCaseFolder, VWTinterfaceDriver * mainDriver):
    QObject((QObject *) mainDriver)
{
    caseFolder = newCaseFolder;
    theDriver = mainDriver;

    QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                     this, SLOT(caseFolderRemoved()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileSystemChange()),
                     this, SLOT(underlyingFilesUpdated()));
    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    underlyingFilesUpdated();
}

CFDcaseInstance::CFDcaseInstance(CFDanalysisType * caseType, VWTinterfaceDriver *mainDriver):
    QObject((QObject *) mainDriver)
{
    myType = caseType;
    theDriver = mainDriver;

    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileSystemChange()),
                     this, SLOT(underlyingFilesUpdated()));
    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));
}

bool CFDcaseInstance::isDefunct()
{
    return defunct;
}

CaseState CFDcaseInstance::getCaseState()
{
    if (defunct) return CaseState::DEFUNCT;
    if (currentReq != PendingCFDrequest::NONE) return CaseState::AGAVE_RUN;
    return myState;
}

QString CFDcaseInstance::getCaseFolder()
{
    QString ret;
    if (caseFolder == NULL) return ret;
    return caseFolder->getFileData().getFullPath();
}

QString CFDcaseInstance::getCaseName()
{
    QString ret;
    if (caseFolder == NULL) return ret;
    return caseFolder->getFileData().getFileName();
}

CFDanalysisType * CFDcaseInstance::getMyType()
{
    if (defunct) return NULL;
    return myType;
}

QMap<QString, QString> CFDcaseInstance::getCurrentParams()
{
    QMap<QString, QString> ret;
    if (defunct) return ret;
    if (myState != CaseState::READY)
    {
        return ret;
    }

    if ((caseFolder == NULL) || (caseFolder->childIsUnloaded()))
    {
        underlyingFilesUpdated();
        return ret;
    }

    FileTreeNode * vars = caseFolder->getChildNodeWithName(".caseParams");
    if (vars == NULL)
    {
        underlyingFilesUpdated();
        return ret;
    }

    QByteArray * rawVars = vars->getFileBuffer();
    if (rawVars == NULL)
    {
        underlyingFilesUpdated();
        return ret;
    }

    QJsonDocument varDoc = QJsonDocument::fromJson(*rawVars);

    if (varDoc.isNull())
    {
        underlyingFilesUpdated();
        return ret;
    }

    QJsonObject varsList = varDoc.object().value("vars").toObject();

    for (auto itr = varsList.constBegin(); itr != varsList.constEnd(); itr++)
    {
        ret.insert(itr.key(),(*itr).toString());
    }

    return ret;
}

QMap<QString, RemoteJobData * > CFDcaseInstance::getRelevantJobs()
{
    QMap<QString, RemoteJobData> jobs = theDriver->getJobHandler()->getRunningJobs();

    QMap<QString, RemoteJobData * > ret;

    if (caseFolder == NULL)
    {
        if (expectedNewCaseFolder.isEmpty())
        {
            return ret;
        }
    }

    for (auto itr = jobs.begin(); itr != jobs.end(); itr++)
    {
        QString appName = (*itr).getApp();
        if (!appName.contains("cwe-mesh")
                && !appName.contains("cwe-sim")
                && !appName.contains("cwe-post"))
        {
            continue;
        }

        if (!(*itr).detailsLoaded())
        {
            ret.insert(appName, &(*itr));
            theDriver->getJobHandler()->requestJobDetails(&(*itr));
            continue;
        }

        //TODO: Procedure for more precise relevance
        ret.insert(appName, &(*itr));
    }
    return ret;
}

QMap<QString, StageState> CFDcaseInstance::getStageStates()
{
    QMap<QString, StageState> ret;
    if (defunct)
    {
        return ret;
    }

    if (myType == NULL)
    {
        return ret;
    }

    QStringList stateList = myType->getStageNames();

    for (auto itr = stateList.begin(); itr != stateList.cend(); itr++)
    {
        ret.insert((*itr), StageState::LOADING);
    }

    if (caseFolder == NULL)
    {
        return ret;
    }

    //Check job handler for running tasks on this folder
    QMap<QString, RemoteJobData * > relevantJobs = getRelevantJobs();

    for (auto itr = relevantJobs.begin(); itr != relevantJobs.end(); itr++)
    {
        QString appName = itr.key();

        if (appName.contains("cwe-mesh"))
        {
            if (ret.contains("mesh"))
            {
                ret["mesh"] = StageState::RUNNING;
            }
        }
        if (appName.contains("cwe-sim"))
        {
            if (ret.contains("sim"))
            {
                ret["sim"] = StageState::RUNNING;
            }
        }
        if (appName.contains("cwe-post"))
        {
            if (ret.contains("post"))
            {
                ret["post"] = StageState::RUNNING;
            }
        }
    }

    if (caseFolder->childIsUnloaded())
    {
        return ret;
    }

    QList<FileTreeNode *> * childList = caseFolder->getChildList();

    //Check known files for expected result files
    for (auto itr = stateList.begin(); itr != stateList.cend(); itr++)
    {
        if (ret[*itr] == StageState::RUNNING)
        {
            continue;
        }

        FileTreeNode * checkNode = caseFolder->getChildNodeWithName(*itr);
        if (checkNode == NULL)
        {
            ret[*itr] = StageState::UNRUN;
        }
        else
        {
            ret[*itr] = StageState::FINISHED;
        }
    }
    return ret;
}

void CFDcaseInstance::createCase(QString newName, FileTreeNode * containingFolder)
{
    if (defunct) return;

    if (!expectedNewCaseFolder.isEmpty()) return;
    if (caseFolder != NULL) return;

    expectedNewCaseFolder = containingFolder->getFileData().getFullPath();
    expectedNewCaseFolder = expectedNewCaseFolder.append("/");
    expectedNewCaseFolder = expectedNewCaseFolder.append(newName);

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("newFolder", newName);
    rawParams.insert("template", myType->getInternalName());

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-create", rawParams, containingFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void duplicateCase(QString newName, FileTreeNode * containingFolder, FileTreeNode * oldCase)
{
    //TODO
}

void CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return;
    if (caseFolder == NULL) return;

    FileTreeNode * varStore = caseFolder->getChildNodeWithName(".caseParams");

    varStore->setFileBuffer(NULL);

    QMultiMap<QString, QString> rawParams;
    QString paramString;

    qDebug("sending ...\n");
    for (auto itr = paramList.cbegin(); itr != paramList.cend(); itr++)
    {
        qDebug("%s: %s", qPrintable(itr.key()), qPrintable(*itr));
        paramString += (itr.key());
        paramString += " ";
        paramString += (*itr);
        paramString += " ";
    }

    rawParams.insert("params", paramString);

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-update", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void CFDcaseInstance::mesh(FileTreeNode * geoFile)
{
    if (defunct) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("action", "mesh");
    rawParams.insert("inFile", geoFile->getFileData().getFullPath());

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-exec-serial", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void CFDcaseInstance::rollBack(QString stageToDelete)
{
    if (defunct) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("step", stageToDelete);

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-delete", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void CFDcaseInstance::openFOAM()
{
    if (defunct) return;

    QMultiMap<QString, QString> rawParams;
    QMap<QString, QString> caseParams = getCurrentParams();

    //TODO: When new Agave app available, let backend determine solver
    if (caseParams.contains("turbModel") && (caseParams.value("turbModel") != "Laminar"))
    {
        rawParams.insert("solver", "icoFoam");
    }
    else
    {
        rawParams.insert("solver", "pisoFoam");
    }

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-sim", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void CFDcaseInstance::postProcess()
{
    if (defunct) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("action", "post");

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-exec-serial", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_RUN);
}

void CFDcaseInstance::killCaseConnection()
{
    defunct = true;
    emit detachCase();
    this->deleteLater();
}

void CFDcaseInstance::underlyingFilesUpdated()
{
    if (defunct) return;

    if (caseFolder == NULL)
    {
        if (expectedNewCaseFolder.isEmpty())
        {
            emitNewState(CaseState::ERROR);
            return;
        }

        demandFolderSearch();

        if (caseFolder != NULL)
        {
            theDriver->getFileHandler()->enactFolderRefresh(caseFolder);
        }
        emitNewState(CaseState::LOADING);
        return;
    }

    if (caseFolder->childIsUnloaded())
    {
        theDriver->getFileHandler()->enactFolderRefresh(caseFolder);
        emitNewState(CaseState::LOADING);
        return;
    }

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(".caseParams");

    if (varFile == NULL)
    {
        emitNewState(CaseState::INVALID);
        return;
    }

    QByteArray * varStore = varFile->getFileBuffer();
    if (varStore == NULL)
    {
        theDriver->getFileHandler()->sendDownloadBuffReq(varFile);
        emitNewState(CaseState::LOADING);
        return;
    }

    if (myType == NULL)
    {
        QJsonDocument varDoc = QJsonDocument::fromJson(*varStore);
        QString templateName = varDoc.object().value("type").toString();
        if (templateName.isEmpty())
        {
            emitNewState(CaseState::ERROR);
            return;
        }

        QList<CFDanalysisType *> * templates = theDriver->getTemplateList();
        for (auto itr = templates->cbegin(); (itr != templates->cend()) && (myType == NULL); itr++)
        {
            if (templateName == (*itr)->getInternalName())
            {
                myType = (*itr);
            }
        }

        if (myType == NULL)
        {
            emitNewState(CaseState::ERROR);
            return;
        }
    }

    //Enact LS of folders from list of check files in configuration
    QJsonObject stages = myType->getRawConfig()->object().value("stages").toObject();

    for (auto itr = stages.constBegin(); itr != stages.constEnd(); itr++)
    {
        QString stageExpect = (*itr).toObject().value("expected").toString();

        if (stageExpect == "<NUMERICAL>")
        {
            continue;
        }

        QString searchPath = caseFolder->getFileData().getFullPath();
        searchPath = searchPath.append("/");
        searchPath = searchPath.append(stageExpect);
        FileTreeNode * toFind = theDriver->getFileHandler()->getClosestNodeFromName(searchPath);

        if (!toFind->fileNameMatches(searchPath))
        {
            if (toFind->childIsUnloaded())
            {
                theDriver->getFileHandler()->enactFolderRefresh(toFind);
                emitNewState(CaseState::LOADING);
                return;
            }
        }
    }

    emitNewState(CaseState::READY);
}

void CFDcaseInstance::jobListUpdated()
{
    if (defunct) return;

    if (!getRelevantJobs().isEmpty())
    {
        emitNewState(CaseState::AGAVE_RUN);
        return;
    }

    if (caseFolder == NULL)
    {
        if (expectedNewCaseFolder.isEmpty())
        {
            emitNewState(CaseState::ERROR);
            return;
        }

        demandFolderSearch();
    }

    if (caseFolder != NULL)
    {
        theDriver->getFileHandler()->enactFolderRefresh(caseFolder);
    }

    emitNewState(CaseState::LOADING);
}

void CFDcaseInstance::appInvokeDone(RequestState invokeStatus)
{
    if (defunct) return;

    theDriver->getJobHandler()->demandJobDataRefresh();

    emitNewState(CaseState::LOADING);
}

void CFDcaseInstance::agaveTaskDone(RequestState invokeStatus)
{

}

void CFDcaseInstance::demandFolderSearch()
{
    FileOperator * fileHandle = theDriver->getFileHandler();
    caseFolder = fileHandle->getNodeFromName(expectedNewCaseFolder);

    if (caseFolder == NULL)
    {
        fileHandle->lsClosestNode(expectedNewCaseFolder);
        return;
    }
    else
    {
        //If setting file node, connect caseFolderRemoved slot
        expectedNewCaseFolder.clear();
        QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                         this, SLOT(caseFolderRemoved()));
    }
}

void CFDcaseInstance::caseFolderRemoved()
{
    killCaseConnection();
    theDriver->setCurrentCase(NULL);
}

void CFDcaseInstance::emitNewState(CaseState newState)
{
    if (currentReq != PendingCFDrequest::NONE)
    {
        newState = CaseState::AGAVE_RUN;
    }

    if (newState == myState) return;
    myState = newState;
    emit haveNewState(newState);
}

void CFDcaseInstance::displayNetError(QString infoText)
{
    QMessageBox infoMessage;
    infoMessage.setText(infoText);
    infoMessage.setIcon(QMessageBox::Information);
    infoMessage.exec();
}
