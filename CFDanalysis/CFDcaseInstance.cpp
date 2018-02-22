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
    QObject::connect(caseFolder, SIGNAL(fileDataChanged()),
                     this, SLOT(underlyingFilesUpdated()));
    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    if (theDriver->inOfflineMode())
    {
        myState = InternalCaseState::OFFLINE;
    }
    else
    {
        myState = InternalCaseState::INIT_DATA_LOAD;
        enactDataReload();
    }
}

CFDcaseInstance::CFDcaseInstance(CFDanalysisType * caseType, VWTinterfaceDriver *mainDriver):
    QObject((QObject *) mainDriver)
{
    myType = caseType;
    theDriver = mainDriver;

    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    if (theDriver->inOfflineMode())
    {
        myState = InternalCaseState::OFFLINE;
    }
    else
    {
        myState = InternalCaseState::TYPE_SELECTED;
    }
}

CFDcaseInstance::CFDcaseInstance(VWTinterfaceDriver *mainDriver):
    QObject((QObject *) mainDriver)
{
    theDriver = mainDriver;

    QObject::connect(theDriver->getJobHandler(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()));
    QObject::connect(theDriver->getFileHandler(), SIGNAL(fileOpDone(RequestState)),
                     this, SLOT(agaveTaskDone(RequestState)));

    if (theDriver->inOfflineMode())
    {
        myState = InternalCaseState::OFFLINE;
    }
    else
    {
        myState = InternalCaseState::EMPTY_CASE;
    }
}

bool CFDcaseInstance::isDefunct()
{
    return defunct;
}

CaseState CFDcaseInstance::getCaseState()
{
    if (defunct) return CaseState::DEFUNCT;
    switch (myState)
    {
    case InternalCaseState::DEFUNCT : return CaseState::DEFUNCT;
    case InternalCaseState::DOWNLOAD : return CaseState::DOWNLOAD;
    case InternalCaseState::ERROR : return CaseState::ERROR;
    case InternalCaseState::OFFLINE : return CaseState::OFFLINE;
    case InternalCaseState::READY : return CaseState::READY;
    case InternalCaseState::INVALID : return CaseState::INVALID;
    case InternalCaseState::EMPTY_CASE :
    case InternalCaseState::INIT_DATA_LOAD :
    case InternalCaseState::RE_DATA_LOAD :
    case InternalCaseState::TYPE_SELECTED : return CaseState::LOADING;
    case InternalCaseState::COPYING_FOLDER :
    case InternalCaseState::FOLDER_CHECK_STOPPED_JOB :
    case InternalCaseState::INIT_PARAM_UPLOAD :
    case InternalCaseState::MAKING_FOLDER :
    case InternalCaseState::RUNNING_JOB :
    case InternalCaseState::STARTING_JOB :
    case InternalCaseState::STOPPING_JOB :
    case InternalCaseState::USER_PARAM_UPLOAD :
    case InternalCaseState::WAITING_FOLDER_DEL : return CaseState::RUNNING;
    default:
        return CaseState::ERROR;
    }
    return CaseState::ERROR;
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
    return storedParamList;
}

QMap<QString, StageState> CFDcaseInstance::getStageStates()
{
    QMap<QString, StageState> ret;
    if (defunct) return ret;
    return storedStageStates;
}

void CFDcaseInstance::createCase(QString newName, FileTreeNode * containingFolder)
{
    if (defunct) return;

    if (myState != InternalCaseState::TYPE_SELECTED) return;
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

    emitNewState(InternalCaseState::MAKING_FOLDER);
}

void CFDcaseInstance::duplicateCase(QString newName, FileTreeNode * containingFolder, FileTreeNode * oldCase)
{
    if (defunct) return;

    if (myState == InternalCaseState::EMPTY_CASE) return;
    if (containingFolder == NULL) return;
    if (oldCase == NULL) return;
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

    emitNewState(InternalCaseState::COPYING_FOLDER);
}

void CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return;

    if (myState != InternalCaseState::READY) return;
    if (caseFolder == NULL) return;
    if (theDriver->getFileHandler()->operationIsPending()) return;

    FileTreeNode * varStore = caseFolder->getChildNodeWithName(".caseParams");
    QByteArray * fileData = varStore->getFileBuffer();

    prospectiveNewParamList.clear();

    if (fileData != NULL)
    {
        QJsonDocument varDoc = QJsonDocument::fromJson(*fileData);
        if (!varDoc.isNull())
        {
            QJsonObject varsList = varDoc.object().value("vars").toObject();

            for (auto itr = varsList.constBegin(); itr != varsList.constEnd(); itr++)
            {
                prospectiveNewParamList.insert(itr.key(),(*itr).toString());
            }
        }
    }

    for (auto itr = paramList.constBegin(); itr != paramList.constEnd(); itr++)
    {
        if (prospectiveNewParamList.contains(itr.key()))
        {
            prospectiveNewParamList[itr.key()] = (*itr);
        }
        else
        {
            prospectiveNewParamList.insert(itr.key(),(*itr));
        }
    }

    QByteArray newFile = produceJSONparams(prospectiveNewParamList);
    theDriver->getFileHandler()->sendUploadBuffReq(caseFolder, newFile, ".caseParams");
    if (!theDriver->getFileHandler()->operationIsPending())
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }

    varStore->setFileBuffer(NULL);
    emitNewState(InternalCaseState::USER_PARAM_UPLOAD);
}

void CFDcaseInstance::startStageApp(QString stageID)
{
    if (defunct) return;
    if (caseFolder == NULL) return;
    if (myType == NULL) return;
    if (myState != InternalCaseState::READY) return;
    if (storedStageStates.value(stageID, StageState::ERROR) != StageState::UNRUN) return;

    QString appName = myType->getStageApp(stageID);

    QMultiMap<QString, QString> rawParams;
    rawParams.insert("stage",stageID);
    QString extraInput = myType->getExtraInput(stageID);
    if (!extraInput.isEmpty())
    {
        QString addedInputVal = getCurrentParams().value(extraInput);
        if (!addedInputVal.isEmpty())
        {
            rawParams.insert("file_input", addedInputVal);
        }
    }

    RemoteDataInterface * remoteConnect = theDriver->getDataConnection();
    QString jobName = appName;
    jobName = jobName.append("-");
    jobName = jobName.append(stageID);
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob(appName, rawParams, caseFolder->getFileData().getFullPath(), jobName);

    if (jobHandle == NULL)
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        return;
    }
    runningStage = stageID;
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(agaveTaskDone(RequestState)));
    emitNewState(InternalCaseState::STARTING_JOB);
}

void CFDcaseInstance::rollBack(QString stageToDelete)
{
    if (defunct) return;
    if (caseFolder == NULL) return;
    if (myState != InternalCaseState::READY) return;
    if (storedStageStates.value(stageToDelete, StageState::ERROR) != StageState::FINISHED) return;
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

    runningStage = stageToDelete;
    emitNewState(InternalCaseState::WAITING_FOLDER_DEL);
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
    //TODO : Recursive download
}

void CFDcaseInstance::stopJob(QString stage)
{
    //TODO: Redo this
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
    RemoteJobData * theJob = relevantJobs.first();

    if (theJob->detailsLoaded() == false)
    {
        cwe_globals::displayPopup("Need to reload job list before job can be stopped, please wait.", "Network Issue");
        return;
    }

    if (theJob->getParams().value("stage") != stage)
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
    processInternalStateInput(StateChangeType::NEW_FILE_DATA, RequestState::GOOD);
}

void CFDcaseInstance::jobListUpdated()
{
    if (defunct) return;
    processInternalStateInput(StateChangeType::NEW_JOB_LIST, RequestState::GOOD);
}

void CFDcaseInstance::agaveTaskDone(RequestState invokeStatus)
{
    if (defunct) return;
    processInternalStateInput(StateChangeType::REMOTE_OP_DONE, invokeStatus);
}

void CFDcaseInstance::caseFolderRemoved()
{
    killCaseConnection();
    theDriver->setCurrentCase(NULL);
}

void CFDcaseInstance::emitNewState(InternalCaseState newState)
{
    if (defunct) return;

    if (newState != myState)
    {
        myState = newState;
        storedStageStates = computeStageStates();
        emit haveNewState(newState);
        return;
    }

    QMap<QString, StageState> oldStageStates = storedStageStates;
    storedStageStates = computeStageStates();

    if (!stageStatesEqual(&oldStageStates, &storedStageStates))
    {
        emit haveNewStageStates();
    }
}

void CFDcaseInstance::processInternalStateInput(StateChangeType theChange, RequestState invokeStatus)
{
    if (defunct) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(CaseState::ERROR);
        cwe_globals::displayPopup("Unable to contact DesignSafe. Connection may have been lost. Please reset and try again.", "Network Issue");
        return;
    }

    InternalCaseState newState = myState;

    if (myState == InternalCaseState::COPYING_FOLDER)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            theDriver->getFileHandler()->speculateNodeWithName(expectedNewCaseFolder, true);
            caseFolder = theDriver->getFileHandler()->getNodeFromName(expectedNewCaseFolder);
            if (caseFolder == NULL)
            {
                internalStateError();
                return;
            }
            expectedNewCaseFolder.clear();
            QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                             this, SLOT(caseFolderRemoved()));
            QObject::connect(caseFolder, SIGNAL(fileDataChanged()),
                             this, SLOT(underlyingFilesUpdated()));

            enactDataReload();
            newState = InternalCaseState::INIT_DATA_LOAD;
        }
    }
    else if (myState == InternalCaseState::DOWNLOAD)
    {
        //TODO: Recursive download
    }
    else if (myState == InternalCaseState::FOLDER_CHECK_STOPPED_JOB)
    {
        if (theChange == StateChangeType::NEW_FILE_DATA)
        {
            //TODO
        }
    }
    else if (myState == InternalCaseState::INIT_DATA_LOAD)
    {
        if ((theChange == StateChangeType::NEW_FILE_DATA) || (theChange == StateChangeType::NEW_JOB_LIST))
        {
            //TODO
        }
    }
    else if (myState == InternalCaseState::INIT_PARAM_UPLOAD)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            //TODO
        }
    }
    else if (myState == InternalCaseState::MAKING_FOLDER)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            theDriver->getFileHandler()->speculateNodeWithName(expectedNewCaseFolder, true);
            caseFolder = theDriver->getFileHandler()->getNodeFromName(expectedNewCaseFolder);
            if (caseFolder == NULL)
            {
                internalStateError();
                return;
            }
            expectedNewCaseFolder.clear();
            QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                             this, SLOT(caseFolderRemoved()));
            QObject::connect(caseFolder, SIGNAL(fileDataChanged()),
                             this, SLOT(underlyingFilesUpdated()));

            QMap<QString, QString> allVars;
            QByteArray newFile = produceJSONparams(allVars);

            theDriver->getFileHandler()->sendUploadBuffReq(caseFolder, newFile, ".caseParams");

            if (!theDriver->getFileHandler()->operationIsPending())
            {
                cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
                return;
            }
            newState = InternalCaseState::INIT_PARAM_UPLOAD;
        }
    }
    else if (myState == InternalCaseState::READY)
    {
        //Note: We may want to consider more fully what happens if underlying files changed outside of CWE
        if (theChange != StateChangeType::NEW_JOB_LIST)
        {
            internalStateError();
            return;
        }
    }
    else if (myState == InternalCaseState::RE_DATA_LOAD)
    {
        if ((theChange == StateChangeType::NEW_FILE_DATA) || (theChange == StateChangeType::NEW_JOB_LIST))
        {
            //TODO
        }
    }
    else if (myState == InternalCaseState::RUNNING_JOB)
    {
        if (theChange == StateChangeType::NEW_JOB_LIST)
        {
            QMap<QString, RemoteJobData * > jobList = getRelevantJobs();

            bool foundRelevantTask = false;

            for (auto itr = jobList.cbegin(); (itr != jobList.cend()) && (foundRelevantTask == false); itr++)
            {
                RemoteJobData * aJob = (*itr);
                if (aJob->detailsLoaded())
                {
                    if (caseFolder->fileNameMatches(aJob->getInputs().value("directory")))
                    {
                        foundRelevantTask = true;
                    }
                }
                else
                {
                    foundRelevantTask = true;
                }
            }

            if (foundRelevantTask == false)
            {
                enactDataReload();
                newState = InternalCaseState::RE_DATA_LOAD;
            }
        }
    }
    else if (myState == InternalCaseState::STARTING_JOB)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            theDriver->getJobHandler()->demandJobDataRefresh();
            newState = InternalCaseState::RUNNING_JOB;
        }
    }
    else if (myState == InternalCaseState::STOPPING_JOB)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            theDriver->getFileHandler()->enactFolderRefresh(caseFolder, true);
            newState = InternalCaseState::FOLDER_CHECK_STOPPED_JOB;
        }
    }
    else if (myState == InternalCaseState::USER_PARAM_UPLOAD)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            FileTreeNode * paramNode = caseFolder->getChildNodeWithName(".caseParams");
            if (paramNode != NULL)
            {
                paramNode->setFileBuffer(NULL);
            }
            enactDataReload();
            newState = InternalCaseState::RE_DATA_LOAD;
        }
    }
    else if (myState == InternalCaseState::WAITING_FOLDER_DEL)
    {
        if (theChange == StateChangeType::REMOTE_OP_DONE)
        {
            theDriver->getFileHandler()->enactFolderRefresh(caseFolder, true);
            enactDataReload();
            newState = InternalCaseState::RE_DATA_LOAD;
        }
    }

    emit emitNewState(newState);
}

//DOLINE

void CFDcaseInstance::enactDataReload()
{
    //TODO

    /*
     * FileTreeNode * varFile = caseFolder->getChildNodeWithName(".caseParams");

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
    */
}

void CFDcaseInstance::internalStateError()
{
    //TODO
}

bool CFDcaseInstance::stageStatesEqual(QMap<QString, StageState> *list1, QMap<QString, StageState> *list2)
{
    if (list1->size() != list2->size())
    {
        return false;
    }

    for (auto itr = list1->cbegin(); itr != list1->cend(); itr++)
    {
        if (!list2->contains(itr.key()))
        {
            return false;
        }
        if (*itr != list2->value(itr.key()))
        {
            return false;
        }
    }
    return true;
}

QMap<QString, StageState> CFDcaseInstance::computeStageStates()
{
    //TODO: Rewrite for new states
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
//TODO: Need to clarify the following line
    if (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED)
    {
        return ret;
    }

    //Check job handler for running tasks on this folder
    QMap<QString, RemoteJobData * > relevantJobs = getRelevantJobs();

    for (auto itr = relevantJobs.cbegin(); itr != relevantJobs.cend(); itr++)
    {
        if (!(*itr)->detailsLoaded())
        {
            return ret;
        }
    }

    for (auto itr = relevantJobs.cbegin(); itr != relevantJobs.cend(); itr++)
    {
        RemoteJobData * theJob = *itr;
        QString stageName = theJob->getParams().value("stage");
        if (ret.contains(stageName))
        {
            ret[stageName] = StageState::RUNNING;
        }
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

void CFDcaseInstance::computeParamList()
{
    if (defunct) return;
    if (myState != CaseState::READY)
    {
        return;
    }
    //TODO: Rewrite this so that demand for data refresh is done elsewhere
    if ((caseFolder == NULL) || (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED))
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
        if (!appName.contains("cwe-serial") && !appName.contains("cwe-parallel"))
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

