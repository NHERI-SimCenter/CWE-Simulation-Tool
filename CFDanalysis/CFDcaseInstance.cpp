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
#include "../AgaveExplorer/remoteFileOps/joboperator.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/remotejobdata.h"

#include "vwtinterfacedriver.h"
#include "cwe_globals.h"

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

    if (theDriver->inOfflineMode())
    {
        myState = CaseState::OFFLINE;
    }
}

CFDcaseInstance::CFDcaseInstance(VWTinterfaceDriver *mainDriver):
    QObject((QObject *) mainDriver)
{
    theDriver = mainDriver;

    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileSystemChange()),
                     this, SLOT(underlyingFilesUpdated()));
    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    if (theDriver->inOfflineMode())
    {
        myState = CaseState::OFFLINE;
    }
}

bool CFDcaseInstance::isDefunct()
{
    return defunct;
}

CaseState CFDcaseInstance::getCaseState()
{
    if (defunct) return CaseState::DEFUNCT;
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

QString CFDcaseInstance::currentAgaveRequest()
{
    QString nil;
    if (defunct) return nil;
    if (currentReq == PendingCFDrequest::NONE)
    {
        return nil;
    }
    if (currentReq == PendingCFDrequest::CREATE_MKDIR)
    {
        return "Creating new case folder";
    }
    if (currentReq == PendingCFDrequest::CREATE_UPLOAD)
    {
        return "Initializing Parameters file";
    }
    if (currentReq == PendingCFDrequest::DUP_COPY)
    {
        return "Copying case folder";
    }
    if (currentReq == PendingCFDrequest::PARAM_UPLOAD)
    {
        return "Uploading new parameter file";
    }
    if (currentReq == PendingCFDrequest::APP_INVOKE)
    {
        return "Invoking stage app";
    }
    if (currentReq == PendingCFDrequest::APP_RUN)
    {
        return "Running stage app";
    }
    if (currentReq == PendingCFDrequest::ROLLBACK_DEL)
    {
        return "Deleting old folder";
    }
    return nil;
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
    QMap<QString, RemoteJobData * > ret;

    if (caseFolder == NULL)
    {
        return ret;
    }

    QMap<QString, RemoteJobData> jobs = theDriver->getJobHandler()->getRunningJobs();

    for (auto itr = jobs.begin(); itr != jobs.end(); itr++)
    {
        QString jobID = (*itr).getID();
        QString appName = (*itr).getApp();
        if (!appName.contains("cwe-mesh")
                && !appName.contains("cwe-sim")
                && !appName.contains("cwe-post"))
        {
            continue;
        }

        if (!(*itr).detailsLoaded())
        {
            ret.insert(jobID, &(*itr));
            theDriver->getJobHandler()->requestJobDetails(&(*itr));
            continue;
        }

        QString jobDir = (*itr).getParams().value("directory");

        if (caseFolder->fileNameMatches(jobDir))
        {
            ret.insert(jobID, &(*itr));
        }
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

    if (relevantJobs.size() > 1)
    {
        for (auto itr = stateList.begin(); itr != stateList.cend(); itr++)
        {
            ret[*itr] = StageState::LOADING;
        }
        return ret;
    }

    if (relevantJobs.size() == 1)
    {
        QString appName = relevantJobs.first()->getID();
        if (appName.contains("cwe-mesh"))
        {
            if (ret.contains("mesh"))
            {
                ret["mesh"] = StageState::RUNNING;
            }
            if (ret.contains("sim"))
            {
                ret["sim"] = StageState::UNRUN;
            }
            if (ret.contains("post"))
            {
                ret["post"] = StageState::UNRUN;
            }
        }
        if (appName.contains("cwe-sim"))
        {
            if (ret.contains("mesh"))
            {
                ret["mesh"] = StageState::FINISHED;
            }
            if (ret.contains("sim"))
            {
                ret["sim"] = StageState::RUNNING;
            }
            if (ret.contains("post"))
            {
                ret["post"] = StageState::UNRUN;
            }
        }
        if (appName.contains("cwe-post"))
        {
            if (ret.contains("mesh"))
            {
                ret["mesh"] = StageState::FINISHED;
            }
            if (ret.contains("sim"))
            {
                ret["sim"] = StageState::FINISHED;
            }
            if (ret.contains("post"))
            {
                ret["post"] = StageState::RUNNING;
            }
        }
        return ret;
    }

    if (caseFolder->childIsUnloaded())
    {
        return ret;
    }

    //Check known files for expected result files
    for (auto itr = stateList.begin(); itr != stateList.cend(); itr++)
    {
        if (ret[*itr] != StageState::LOADING)
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

    if (myState == CaseState::OFFLINE) return;

    if (currentReq != PendingCFDrequest::NONE) return;
    if (theDriver->getFileHandler()->operationIsPending()) return;
    if (caseFolder != NULL) return;
    if (!expectedNewCaseFolder.isEmpty()) return;

    expectedNewCaseFolder = containingFolder->getFileData().getFullPath();
    expectedNewCaseFolder = expectedNewCaseFolder.append("/");
    expectedNewCaseFolder = expectedNewCaseFolder.append(newName);

    theDriver->getFileHandler()->sendCreateFolderReq(containingFolder, newName);

    if (!theDriver->getFileHandler()->operationIsPending())
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    currentReq = PendingCFDrequest::CREATE_MKDIR;
    emitNewState(CaseState::OP_INVOKE);
    requestDataBeingRefreshed = false;
}

void CFDcaseInstance::duplicateCase(QString newName, FileTreeNode * containingFolder, FileTreeNode * oldCase)
{
    if (defunct) return;

    if (myState == CaseState::OFFLINE) return;

    if (containingFolder == NULL) return;
    if (oldCase == NULL) return;
    if (currentReq != PendingCFDrequest::NONE) return;
    if (theDriver->getFileHandler()->operationIsPending()) return;
    if (caseFolder != NULL) return;
    if (!expectedNewCaseFolder.isEmpty()) return;

    expectedNewCaseFolder = containingFolder->getFileData().getFullPath();
    expectedNewCaseFolder = expectedNewCaseFolder.append("/");
    expectedNewCaseFolder = expectedNewCaseFolder.append(newName);

    theDriver->getFileHandler()->sendCopyReq(oldCase, expectedNewCaseFolder);

    if (!theDriver->getFileHandler()->operationIsPending())
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    currentReq = PendingCFDrequest::DUP_COPY;
    emitNewState(CaseState::OP_INVOKE);
    requestDataBeingRefreshed = true;
}

void CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return;
    if (caseFolder == NULL) return;

    if (myState == CaseState::OFFLINE) return;

    if (currentReq != PendingCFDrequest::NONE) return;
    if (theDriver->getFileHandler()->operationIsPending()) return;

    FileTreeNode * varStore = caseFolder->getChildNodeWithName(".caseParams");
    QByteArray * fileData = varStore->getFileBuffer();

    QMap<QString, QString> allVars;

    if (fileData != NULL)
    {
        QJsonDocument varDoc = QJsonDocument::fromJson(*fileData);
        if (!varDoc.isNull())
        {
            QJsonObject varsList = varDoc.object().value("vars").toObject();

            for (auto itr = varsList.constBegin(); itr != varsList.constEnd(); itr++)
            {
                allVars.insert(itr.key(),(*itr).toString());
            }
        }
    }

    for (auto itr = paramList.constBegin(); itr != paramList.constEnd(); itr++)
    {
        if (allVars.contains(itr.key()))
        {
            allVars[itr.key()] = (*itr);
        }
        else
        {
            allVars.insert(itr.key(),(*itr));
        }
    }

    varStore->setFileBuffer(NULL);

    QByteArray newFile = produceJSONparams(allVars);

    theDriver->getFileHandler()->sendUploadBuffReq(caseFolder, newFile, ".caseParams");

    if (!theDriver->getFileHandler()->operationIsPending())
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    currentReq = PendingCFDrequest::PARAM_UPLOAD;
    emitNewState(CaseState::OP_INVOKE);
    requestDataBeingRefreshed = false;
}

void CFDcaseInstance::startStageApp(QString stageID)
{
    if (defunct) return;
    if (caseFolder == NULL) return;

    if (myState == CaseState::OFFLINE) return;

    if (currentReq != PendingCFDrequest::NONE) return;

    QString appName = "cwe-";
    appName = appName.append(stageID);

    QMultiMap<QString, QString> rawParams;

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob(appName, rawParams, caseFolder->getFileData().getFullPath());

    if (jobHandle == NULL)
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(appInvokeDone(RequestState)));
    currentReq = PendingCFDrequest::APP_INVOKE;
    emitNewState(CaseState::JOB_RUN);
    requestDataBeingRefreshed = false;
}

void CFDcaseInstance::rollBack(QString stageToDelete)
{
    //TODO: Also need to rollback subsequent stages eventually.
    if (defunct) return;
    if (caseFolder == NULL) return;

    if (myState == CaseState::OFFLINE) return;

    if (currentReq != PendingCFDrequest::NONE) return;
    if (theDriver->getFileHandler()->operationIsPending()) return;

    FileTreeNode * folderToRemove = caseFolder->getChildNodeWithName(stageToDelete);

    if (folderToRemove == NULL)
    {
        cwe_globals::displayPopup("Unable to remove stage not yet done.", "Network Issue");
        return;
    }

    theDriver->getFileHandler()->sendDeleteReq(folderToRemove);

    if (!theDriver->getFileHandler()->operationIsPending())
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    currentReq = PendingCFDrequest::ROLLBACK_DEL;
    emitNewState(CaseState::OP_INVOKE);
    requestDataBeingRefreshed = false;
}

void CFDcaseInstance::killCaseConnection()
{
    defunct = true;
    emit detachCase();
    this->deleteLater();
}

void CFDcaseInstance::downloadCase(QString destLocalFile)
{
    if (!cwe_globals::isValidLocalFolder(destLocalFile))
    {
        cwe_globals::displayPopup("Please select a valid local folder for download", "I/O Error");
        return;
    }

    cwe_globals::displayPopup("Download debug message", "DEBUG: TODO");
    //TODO : PRS
}

void CFDcaseInstance::stopJob(QString stage)
{
    QMap<QString, RemoteJobData * > relevantJobs = getRelevantJobs();

    if (relevantJobs.size() == 0)
    {
        cwe_globals::displayPopup("No job detected for stopping", "Network Issue");
        return;
    }

    if (relevantJobs.size() > 1)
    {
        cwe_globals::displayPopup("Need to reload job list before job can be stopped, please wait.", "Network Issue");
        return;
    }

    QString jobID = relevantJobs.firstKey();
    QString appName = relevantJobs.first()->getApp();

    bool stopOkay = false;

    //TODO: Can probably clean this logic up, but next version will be more generic for stage names.
    if (appName.contains("cwe-mesh") && (stage == "mesh"))
    {
        stopOkay = true;
    }
    else if (appName.contains("cwe-sim") && (stage == "sim"))
    {
        stopOkay = true;
    }
    else if (appName.contains("cwe-post") && (stage == "post"))
    {
        stopOkay = true;
    }

    if (!stopOkay)
    {
        cwe_globals::displayPopup("Job for deletion not detected.", "Network Issue");
        return;
    }

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    RemoteDataReply * jobHandle = remoteConnect->stopJob(jobID);

    if (jobHandle == NULL)
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    QObject::connect(jobHandle, SIGNAL(haveStoppedJob(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    currentReq = PendingCFDrequest::STOP_JOB;
    emitNewState(CaseState::OP_INVOKE);
    requestDataBeingRefreshed = false;
}

void CFDcaseInstance::underlyingFilesUpdated()
{
    if (defunct) return;

    if (currentReq != PendingCFDrequest::NONE)
    {
        if (requestDataBeingRefreshed == false)
        {
            return;
        }

        if ((currentReq == PendingCFDrequest::CREATE_MKDIR) ||
                 (currentReq == PendingCFDrequest::DUP_COPY))
        {
            if ((caseFolder != NULL) || (expectedNewCaseFolder.isEmpty()))
            {
                emitNewState(CaseState::ERROR);
                cwe_globals::displayPopup("Cannot create new case folder if case already has a folder.", "Network Issue");
                return;
            }

            caseFolder = theDriver->getFileHandler()->getNodeFromName(expectedNewCaseFolder);

            if (caseFolder == NULL)
            {
                theDriver->getFileHandler()->lsClosestNode(expectedNewCaseFolder);
                return;
            }
            else
            {
                //If setting file node, connect caseFolderRemoved slot
                expectedNewCaseFolder.clear();
                QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                                 this, SLOT(caseFolderRemoved()));
                if (currentReq == PendingCFDrequest::DUP_COPY)
                {
                    currentReq = PendingCFDrequest::NONE;
                    theDriver->getFileHandler()->enactFolderRefresh(caseFolder);
                    emitNewState(CaseState::LOADING);
                    return;
                }

                QMap<QString, QString> allVars;
                QByteArray newFile = produceJSONparams(allVars);

                theDriver->getFileHandler()->sendUploadBuffReq(caseFolder, newFile, ".caseParams");

                if (!theDriver->getFileHandler()->operationIsPending())
                {
                    cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
                    return;
                }

                currentReq = PendingCFDrequest::CREATE_UPLOAD;
                emitNewState(CaseState::OP_INVOKE);
                requestDataBeingRefreshed = false;
            }
        }

        return;
    }

    if (caseFolder == NULL)
    {
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

    emitNewState(CaseState::READY);
}

void CFDcaseInstance::jobListUpdated()
{
    if (defunct) return;
    if (caseFolder == NULL) return;
    if (myType == NULL) return;

    QMap<QString, RemoteJobData * > jobList = getRelevantJobs();

    if (jobList.size() >= 1)
    {
        RemoteJobData * theJob = jobList.first();

        QString appName = theJob->getApp();

        if (appName.contains("cwe-mesh") ||
                appName.contains("cwe-sim") ||
                appName.contains("cwe-post"))
        {
            currentReq = PendingCFDrequest::APP_RUN;
        }
        else
        {
            emitNewState(CaseState::ERROR);
            cwe_globals::displayPopup("Case has unrecognized app. Tasks may have been started without this program.", "Network Issue");
            return;
        }
        emitNewState(CaseState::JOB_RUN);
        return;
    }

    if ((currentReq == PendingCFDrequest::APP_INVOKE) ||
            (currentReq == PendingCFDrequest::APP_RUN))
    {
        currentReq = PendingCFDrequest::NONE;
        theDriver->getFileHandler()->enactFolderRefresh(caseFolder);

        emitNewState(CaseState::LOADING);
    }
}

void CFDcaseInstance::appInvokeDone(RequestState invokeStatus)
{
    if (defunct) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(CaseState::ERROR);
        cwe_globals::displayPopup("Unable to contact DesignSafe. Connection may have been lost. Please reset and try again.", "Network Issue");
        return;
    }

    theDriver->getJobHandler()->demandJobDataRefresh();
}

void CFDcaseInstance::agaveTaskDone(RequestState invokeStatus)
{
    if (defunct) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(CaseState::ERROR);
        cwe_globals::displayPopup("Unable to contact DesignSafe. Connection may have been lost. Please reset and try again.", "Network Issue");
        return;
    }

    CaseState newState = CaseState::OP_INVOKE;

    if ((currentReq == PendingCFDrequest::CREATE_MKDIR) || (currentReq == PendingCFDrequest::DUP_COPY))
    {
        requestDataBeingRefreshed = true;
        theDriver->getFileHandler()->lsClosestNode(expectedNewCaseFolder);
    }
    else
    {
        currentReq = PendingCFDrequest::NONE;
        theDriver->getFileHandler()->enactFolderRefresh(caseFolder);

        newState = CaseState::LOADING;
    }
    emitNewState(newState);
}

void CFDcaseInstance::caseFolderRemoved()
{
    killCaseConnection();
    theDriver->setCurrentCase(NULL);
}

QByteArray CFDcaseInstance::produceJSONparams(QMap<QString, QString> paramList)
{
    QJsonDocument ret;
    QJsonObject mainObject;
    ret.setObject(mainObject);

    QJsonValue typeStr(myType->getInternalName());

    mainObject.insert("type",typeStr);

    QJsonObject params;

    for (auto itr = paramList.begin(); itr != paramList.end(); itr++)
    {
        params.insert(itr.key(), (*itr));
    }

    mainObject.insert("vars",params);

    ret.setObject(mainObject);

    return ret.toJson();
}

void CFDcaseInstance::emitNewState(CaseState newState)
{
    if (newState == myState) return;
    myState = newState;
    emit haveNewState(newState);
}
