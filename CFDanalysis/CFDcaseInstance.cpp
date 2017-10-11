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

    myLock = new EasyBoolLock(this);
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

    myLock = new EasyBoolLock(this);
}

bool CFDcaseInstance::isDefunct()
{
    return defunct;
}

CaseState CFDcaseInstance::getCaseState()
{
    if (defunct) return CaseState::DEFUNCT;
    return oldState;
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
    if (caseFolder == NULL) return ret;
    if (caseFolder->childIsUnloaded()) return ret;
    FileTreeNode * vars = caseFolder->getChildNodeWithName(".varStore");
    if (vars == NULL) return ret;

    QByteArray * rawVars = vars->getFileBuffer();
    if (rawVars == NULL) return ret;

    QJsonDocument varDoc = QJsonDocument::fromJson(*rawVars);

    if (varDoc.isNull()) return ret;

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
        if (!appName.contains("cwe-create") && !appName.contains("cwe-dup"))
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
    if (caseFolder == NULL)
    {
        ret.insert("mesh", StageState::LOADING);
        ret.insert("sim", StageState::LOADING);
        return ret;
    }

    //Check job handler for running tasks on this folder
    QMap<QString, RemoteJobData * > relevantJobs = getRelevantJobs();

    for (auto itr = relevantJobs.begin(); itr != relevantJobs.end(); itr++)
    {
        QString appName = itr.key();

        if (appName.contains("cwe-sim"))
        {
            ret.insert("mesh", StageState::FINISHED);
            ret.insert("sim", StageState::RUNNING);
            return ret;
        }
        if (appName.contains("cwe-exec-serial"))
        {
            ret.insert("mesh", StageState::RUNNING);
            ret.insert("sim", StageState::UNRUN);
            return ret;
        }
        if (appName.contains("cwe-update"))
        {
            ret.insert("mesh", StageState::LOADING);
            ret.insert("sim", StageState::LOADING);
            return ret;
        }
        if (appName.contains("cwe-delete"))
        {
            ret.insert("mesh", StageState::LOADING);
            ret.insert("sim", StageState::LOADING);
            return ret;
        }
    }

    //Check known files for expected result files
    QJsonObject stages = myType->getRawConfig()->object().value("stages").toObject();

    for (auto itr = stages.constBegin(); itr != stages.constEnd(); itr++)
    {
        QString stageKey = itr.key();

        QString stageExpect = (*itr).toObject().value("expected").toString();

        if (stageExpect == "<NUMERICAL>")
        {
            if (caseFolder->childIsUnloaded())
            {
                ret.insert(stageKey, StageState::LOADING);
            }
            else
            {
                QList<FileTreeNode *> * childList = caseFolder->getChildList();

                bool hasNumber = false;
                for (auto itr = childList->begin(); (itr != childList->end()) && (hasNumber == false); itr++)
                {
                    QString nameToCheck = (*itr)->getFileData().getFileName();
                    bool ok = false;

                    int intval = nameToCheck.toInt(&ok);

                    if (ok && (intval > 0))
                    {
                        hasNumber = true;
                    }
                }

                if (hasNumber)
                {
                    ret.insert(stageKey, StageState::FINISHED);
                }
                else
                {
                    ret.insert(stageKey, StageState::UNRUN);
                }
            }
        }
        else
        {
            QStringList foldersToSearch = stageExpect.split('/');
            FileTreeNode * currentNode = caseFolder;
            bool foundData = false;
            for (auto itr = foldersToSearch.cbegin(); (itr != foldersToSearch.cend()) && (foundData == false); itr++)
            {
                QString folderName = (*itr);
                if (currentNode->childIsUnloaded())
                {
                    ret.insert(stageKey, StageState::LOADING);
                    foundData = true;
                }
                else
                {
                    currentNode = currentNode->getChildNodeWithName(folderName);
                    if (currentNode == NULL)
                    {
                        ret.insert(stageKey, StageState::UNRUN);
                        foundData = true;
                    }
                }
            }
            if (foundData == false)
            {
                ret.insert(stageKey, StageState::FINISHED);
            }
        }
    }
    return ret;
}

void CFDcaseInstance::createCase(QString newName, FileTreeNode * containingFolder)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
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
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
}

void CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return;
    if (caseFolder == NULL) return;
    if (!myLock->checkAndClaim()) return;

    FileTreeNode * varStore = caseFolder->getChildNodeWithName(".varStore");

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
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
}

void CFDcaseInstance::mesh(FileTreeNode * geoFile)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("action", "mesh");
    rawParams.insert("inFile", geoFile->getFileData().getFullPath());

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-exec-serial", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
}

void CFDcaseInstance::rollBack(QString stageToDelete)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("step", stageToDelete);

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-delete", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
}

void CFDcaseInstance::openFOAM()
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;

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
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
}

void CFDcaseInstance::postProcess()
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("action", "post");

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob("cwe-exec-serial", rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        myLock->release();
        defunct = true;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(remoteCommandDone()));
    emitNewState(CaseState::AGAVE_INVOKE);
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

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(".varStore");

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

void CFDcaseInstance::remoteCommandDone()
{
    if (defunct) return;
    myLock->release();

    theDriver->getJobHandler()->demandJobDataRefresh();

    emitNewState(CaseState::AGAVE_RELOAD);
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
    if (newState == oldState) return;
    CaseState oldoldState = oldState;
    oldState = newState;
    emit haveNewState(oldoldState, oldState);
}
