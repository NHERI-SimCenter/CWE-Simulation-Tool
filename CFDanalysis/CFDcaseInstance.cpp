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
#include "../AgaveExplorer/remoteFileOps/joblistnode.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/remotejobdata.h"

#include "cwe_interfacedriver.h"
#include "cwe_globals.h"

CFDcaseInstance::CFDcaseInstance(FileTreeNode * newCaseFolder):
    QObject((QObject *) cwe_globals::get_CWE_Driver())
{
    caseFolder = newCaseFolder;
    connectCaseSignals();
    computeInitState();
}

CFDcaseInstance::CFDcaseInstance(CFDanalysisType * caseType):
    QObject((QObject *) cwe_globals::get_CWE_Driver())
{
    myType = caseType;
    connectCaseSignals();
    computeInitState();
}

CFDcaseInstance::CFDcaseInstance():
    QObject((QObject *) cwe_globals::get_CWE_Driver())
{
    connectCaseSignals();
    computeInitState();
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
    case InternalCaseState::STARTING_JOB :
    case InternalCaseState::STOPPING_JOB :
    case InternalCaseState::USER_PARAM_UPLOAD :
    case InternalCaseState::WAITING_FOLDER_DEL : return CaseState::OP_INVOKE;
    case InternalCaseState::RUNNING_JOB_NORECORD :
    case InternalCaseState::RUNNING_JOB_YESRECORD : return CaseState::RUNNING;
    default:
        return CaseState::ERROR;
    }
    return CaseState::ERROR;
}

FileTreeNode * CFDcaseInstance::getCaseFolder()
{
    return caseFolder;
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

bool CFDcaseInstance::createCase(QString newName, FileTreeNode * containingFolder)
{
    if (defunct) return false;

    if (myState != InternalCaseState::TYPE_SELECTED) return false;
    if (cwe_globals::get_file_handle()->operationIsPending()) return false;
    if (caseFolder != NULL) return false;
    if (!expectedNewCaseFolder.isEmpty()) return false;

    expectedNewCaseFolder = containingFolder->getFileData().getFullPath();
    expectedNewCaseFolder = expectedNewCaseFolder.append("/");
    expectedNewCaseFolder = expectedNewCaseFolder.append(newName);

    cwe_globals::get_file_handle()->sendCreateFolderReq(containingFolder, newName);

    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        return false;
    }

    emitNewState(InternalCaseState::MAKING_FOLDER);
    return true;
}

bool CFDcaseInstance::duplicateCase(QString newName, FileTreeNode * containingFolder, FileTreeNode * oldCase)
{
    if (defunct) return false;

    if (myState != InternalCaseState::EMPTY_CASE) return false;
    if (containingFolder == NULL) return false;
    if (oldCase == NULL) return false;
    if (cwe_globals::get_file_handle()->operationIsPending()) return false;
    if (caseFolder != NULL) return false;
    if (!expectedNewCaseFolder.isEmpty()) return false;

    expectedNewCaseFolder = containingFolder->getFileData().getFullPath();
    expectedNewCaseFolder = expectedNewCaseFolder.append("/");
    expectedNewCaseFolder = expectedNewCaseFolder.append(newName);

    cwe_globals::get_file_handle()->sendCopyReq(oldCase, expectedNewCaseFolder);

    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        return false;
    }

    emitNewState(InternalCaseState::COPYING_FOLDER);
    return true;
}

bool CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return false;

    if (myState != InternalCaseState::READY) return false;
    if (caseFolder == NULL) return false;
    if (cwe_globals::get_file_handle()->operationIsPending()) return false;

    FileTreeNode * varStore = caseFolder->getChildNodeWithName(caseParamFileName);
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
    cwe_globals::get_file_handle()->sendUploadBuffReq(caseFolder, newFile, caseParamFileName);
    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        return false;
    }

    varStore->setFileBuffer(NULL);
    emitNewState(InternalCaseState::USER_PARAM_UPLOAD);
    return true;
}

bool CFDcaseInstance::startStageApp(QString stageID)
{
    if (defunct) return false;
    if (caseFolder == NULL) return false;
    if (myType == NULL) return false;
    if (myState != InternalCaseState::READY) return false;
    if (storedStageStates.value(stageID, StageState::ERROR) != StageState::UNRUN) return false;

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

    RemoteDataInterface * remoteConnect = cwe_globals::get_Driver()->getDataConnection();
    QString jobName = appName;
    jobName = jobName.append("-");
    jobName = jobName.append(stageID);
    RemoteDataReply * jobHandle = remoteConnect->runRemoteJob(appName, rawParams, caseFolder->getFileData().getFullPath(), jobName);

    if (jobHandle == NULL)
    {
        return false;
    }
    runningStage = stageID;
    runningJobNode = NULL; //Note: job nodes are managed and cleaned up by the job handler
    QObject::connect(jobHandle, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(jobInvoked(RequestState,QJsonDocument*)));
    emitNewState(InternalCaseState::STARTING_JOB);
    return true;
}

bool CFDcaseInstance::rollBack(QString stageToDelete)
{
    if (defunct) return false;
    if (caseFolder == NULL) return false;
    if (myState != InternalCaseState::READY) return false;
    if (storedStageStates.value(stageToDelete, StageState::ERROR) != StageState::FINISHED) return false;
    if (cwe_globals::get_file_handle()->operationIsPending()) return false;

    FileTreeNode * folderToRemove = caseFolder->getChildNodeWithName(stageToDelete);

    if (folderToRemove == NULL) return false;

    cwe_globals::get_file_handle()->sendDeleteReq(folderToRemove);

    if (!cwe_globals::get_file_handle()->operationIsPending()) return false;

    runningStage = stageToDelete;
    emitNewState(InternalCaseState::WAITING_FOLDER_DEL);
    return true;
}

bool CFDcaseInstance::stopJob(QString stage)
{
    if (defunct) return false;
    if (caseFolder == NULL) return false;
    if ((myState != InternalCaseState::RUNNING_JOB_YESRECORD) &&
            (myState != InternalCaseState::RUNNING_JOB_NORECORD)) return false;

    RemoteDataReply * jobHandle = NULL;
    if (myState == InternalCaseState::RUNNING_JOB_YESRECORD)
    {
        if (runningJobNode == NULL) return false;

        if (runningJobNode->getParams().value("stage") != stage) return false;

        jobHandle = cwe_globals::get_connection()->stopJob(runningJobNode->getID());
    }
    else
    {
        jobHandle = cwe_globals::get_connection()->stopJob(runningID);
    }

    if (jobHandle == NULL) return false;

    QObject::connect(jobHandle, SIGNAL(haveStoppedJob(RequestState)),
                     this, SLOT(jobKilled(RequestState)));

    emitNewState(InternalCaseState::STOPPING_JOB);
    return true;
}

bool CFDcaseInstance::downloadCase(QString destLocalFile)
{
    if (defunct) return false;
    if (caseFolder == NULL) return false;
    if (myState != InternalCaseState::READY) return false;
    if (myType == NULL) return false;
    if (cwe_globals::get_file_handle()->operationIsPending()) return false;

    if (!cwe_globals::isValidLocalFolder(destLocalFile))
    {
        cwe_globals::displayPopup("Please select a valid local folder for download", "I/O Error");
        return false;
    }

    FileTreeNode * lastCompleteNode = NULL;

    for (QString aStage : myType->getStageSequence())
    {
        FileTreeNode * testNode = caseFolder->getChildNodeWithName(aStage);
        if (testNode != NULL)
        {
            lastCompleteNode = testNode;
        }
    }

    if (lastCompleteNode == NULL) return false;

    cwe_globals::get_file_handle()->enactRecursiveDownload(lastCompleteNode, destLocalFile);
    if (!cwe_globals::get_file_handle()->operationIsPending()) return false;

    emitNewState(InternalCaseState::DOWNLOAD);
    return true;
}

void CFDcaseInstance::killCaseConnection()
{
    defunct = true;
    cwe_globals::get_CWE_Driver()->caseDetached(this);
    QObject::disconnect(this);
    this->deleteLater();
}

void CFDcaseInstance::underlyingFilesUpdated(FileTreeNode * changedNode, FileSystemChange theChange)
{
    if (defunct) return;
    if ((theChange == FileSystemChange::FILE_DELETE) && (caseFolder == changedNode))
    {
        killCaseConnection();
        return;
    }

    if (!changedNode->isChildOf(caseFolder)) return;

    InternalCaseState activeState = myState;

    switch (activeState)
    {
    case InternalCaseState::FOLDER_CHECK_STOPPED_JOB:
        state_FolderCheckStopped_fileChange_taskDone(); return;

    case InternalCaseState::INIT_DATA_LOAD:
    case InternalCaseState::RE_DATA_LOAD:
        state_DataLoad_fileChange_jobList(); return;

    case InternalCaseState::READY:
        state_Ready_fileChange_jobList(); return;

    default:
        return;
    }
}

void CFDcaseInstance::jobListUpdated()
{
    if (defunct) return;

    InternalCaseState activeState = myState;

    switch (activeState)
    {
    case InternalCaseState::INIT_DATA_LOAD:
    case InternalCaseState::RE_DATA_LOAD:
        state_DataLoad_fileChange_jobList(); return;

    case InternalCaseState::READY:
        state_Ready_fileChange_jobList(); return;

    case InternalCaseState::RUNNING_JOB_NORECORD:
        state_RunningNoRecord_jobList(); return;

    case InternalCaseState::RUNNING_JOB_YESRECORD:
        state_RunningYesRecord_jobList(); return;

    default:
        return;
    }
}

void CFDcaseInstance::fileTaskDone(RequestState invokeStatus, QString opMessage)
{
    if (defunct) return;

    if (invokeStatus == RequestState::NO_CONNECT)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Lost connection to DesignSafe. Please check network and try again.", "Network Connection Error");
        return;
    }

    if ((invokeStatus == RequestState::FAIL) && (!opMessage.isEmpty()))
    {
        cwe_globals::displayPopup(opMessage, "Task Error");
    }

    InternalCaseState activeState = myState;

    switch (activeState)
    {
    case InternalCaseState::COPYING_FOLDER:
        state_CopyingFolder_taskDone(invokeStatus); return;

    case InternalCaseState::FOLDER_CHECK_STOPPED_JOB:
        state_FolderCheckStopped_fileChange_taskDone(); return;

    case InternalCaseState::INIT_PARAM_UPLOAD:
        state_InitParam_taskDone(invokeStatus); return;

    case InternalCaseState::MAKING_FOLDER:
        state_MakingFolder_taskDone(invokeStatus); return;

    case InternalCaseState::USER_PARAM_UPLOAD:
        state_UserParamUpload_taskDone(invokeStatus); return;

    case InternalCaseState::WAITING_FOLDER_DEL:
        state_WaitingFolderDel_taskDone(invokeStatus); return;

    case InternalCaseState::DOWNLOAD:
        state_Download_recursiveOpDone(invokeStatus); return;

    default:
        return;
    }
}

void CFDcaseInstance::chainedStateTransition()
{
    if (defunct) return;

    InternalCaseState activeState = myState;

    switch (activeState)
    {
    case InternalCaseState::FOLDER_CHECK_STOPPED_JOB:
        state_FolderCheckStopped_fileChange_taskDone(); return;

    case InternalCaseState::INIT_DATA_LOAD:
    case InternalCaseState::RE_DATA_LOAD:
        state_DataLoad_fileChange_jobList(); return;

    case InternalCaseState::READY:
        state_Ready_fileChange_jobList(); return;

    case InternalCaseState::RUNNING_JOB_NORECORD:
        state_RunningNoRecord_jobList(); return;

    case InternalCaseState::RUNNING_JOB_YESRECORD:
        state_RunningYesRecord_jobList(); return;

    default:
        return;
    }
}

void CFDcaseInstance::jobInvoked(RequestState invokeStatus, QJsonDocument *jobData)
{
    if (defunct) return;

    if (invokeStatus == RequestState::NO_CONNECT)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Lost connection to DesignSafe. Please check network and try again.", "Network Connection Error");
        return;
    }

    if (invokeStatus == RequestState::FAIL)
    {
        emitNewState(InternalCaseState::RE_DATA_LOAD);
        cwe_globals::displayPopup("Unable to start requested job. Reloading Case", "Network Connection Error");
        enactDataReload();
        return;
    }

    InternalCaseState activeState = myState;

    QString idFromReply = jobData->object().value("result").toObject().value("id").toString();

    switch (activeState)
    {
    case InternalCaseState::STARTING_JOB:
        state_StartingJob_jobInvoked(idFromReply); return;

    default:
        return;
    }
}

void CFDcaseInstance::jobKilled(RequestState invokeStatus)
{
    if (defunct) return;

    if (invokeStatus == RequestState::NO_CONNECT)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Lost connection to DesignSafe. Please check network and try again.", "Network Connection Error");
        return;
    }

    if (invokeStatus == RequestState::FAIL)
    {
        emitNewState(InternalCaseState::RE_DATA_LOAD);
        cwe_globals::displayPopup("Unable to stop requested job. Reloading Case", "Network Connection Error");
        enactDataReload();
        return;
    }

    InternalCaseState activeState = myState;

    switch (activeState)
    {
    case InternalCaseState::STOPPING_JOB:
        state_StoppingJob_jobKilled(); return;

    default:
        return;
    }
}

void CFDcaseInstance::computeInitState()
{
    if (cwe_globals::get_CWE_Driver()->inOfflineMode())
    {
        myState = InternalCaseState::OFFLINE;
        return;
    }

    if (caseFolder == NULL)
    {
        if (myType == NULL)
        {
            myState = InternalCaseState::EMPTY_CASE;
        }
        else
        {
            myState = InternalCaseState::TYPE_SELECTED;
        }
        return;
    }

    myState = InternalCaseState::INIT_DATA_LOAD;
    recomputeStageStates();
    chainedStateTransition();
}

void CFDcaseInstance::emitNewState(InternalCaseState newState)
{
    if (defunct) return;

    if (myState == InternalCaseState::ERROR) return;

    if (newState != myState)
    {
        myState = newState;
        recomputeStageStates();
        emit haveNewState(getCaseState());
        return;
    }

    if (recomputeStageStates())
    {
        emit haveNewState(getCaseState());
    }
}

void CFDcaseInstance::enactDataReload()
{
    if (defunct) return;
    if (caseDataLoaded()) return;
    FileTreeNode * varFile = caseFolder->getChildNodeWithName(caseParamFileName);

    if (varFile == NULL)
    {
        cwe_globals::get_file_handle()->speculateNodeWithName(caseFolder, caseParamFileName, false);
        return;
    }

    QByteArray * varStore = varFile->getFileBuffer();
    if (varStore == NULL)
    {
        cwe_globals::get_file_handle()->sendDownloadBuffReq(varFile);
    }
    return;
}

bool CFDcaseInstance::caseDataLoaded()
{
    if (defunct) return false;
    if (myType == NULL) return false;
    if (caseFolder == NULL) return false;

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(caseParamFileName);

    if (varFile == NULL) return false;
    QByteArray * varStore = varFile->getFileBuffer();
    if (varStore == NULL) return false;

    if (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED) return false;
    if (varFile->getNodeState() != NodeState::FILE_BUFF_LOADED) return false;

    return true;
}

bool CFDcaseInstance::caseDataInvalid()
{
    //This is for when we have loaded the remote info and we know this current folder is NOT a CWE case
    if (defunct) return false;
    if (caseFolder == NULL) return false;

    if (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED) return false;

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(caseParamFileName);

    if (varFile == NULL) return true;
    if (varFile->getNodeState() != NodeState::FILE_BUFF_LOADED) return false;

    QByteArray * varStore = varFile->getFileBuffer();
    if (varStore == NULL) return true;

    QJsonDocument varDoc = QJsonDocument::fromJson(*varStore);
    QString templateName = varDoc.object().value("type").toString();
    if (templateName.isEmpty())
    {
        return true;
    }

    QList<CFDanalysisType *> * templates = cwe_globals::get_CWE_Driver()->getTemplateList();
    for (auto itr = templates->cbegin(); itr != templates->cend(); itr++)
    {
        if (templateName == (*itr)->getInternalName())
        {
            return false;
        }
    }

    return true;
}

void CFDcaseInstance::computeCaseType()
{
    if (myType != NULL) return;

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(caseParamFileName);
    if (varFile == NULL) return;

    QByteArray * varStore = varFile->getFileBuffer();
    if (varStore == NULL) return;

    QJsonDocument varDoc = QJsonDocument::fromJson(*varStore);
    QString templateName = varDoc.object().value("type").toString();
    if (templateName.isEmpty())
    {
        return;
    }

    QList<CFDanalysisType *> * templates =cwe_globals::get_CWE_Driver()->getTemplateList();
    for (auto itr = templates->cbegin(); (itr != templates->cend()) && (myType == NULL); itr++)
    {
        if (templateName == (*itr)->getInternalName())
        {
            myType = (*itr);
            return;
        }
    }
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

bool CFDcaseInstance::updateStageStatesIfNew(QMap<QString, StageState> * newStageStates)
{
    if (stageStatesEqual(newStageStates, &storedStageStates))
    {
        return false;
    }
    storedStageStates = *newStageStates;
    return true;
}

bool CFDcaseInstance::recomputeStageStates()
{
    //Return true if stage states have changed

    //Note: The code here smells bad
    //Needs re-think and re-write

    if (defunct) return false;
    if (myType == NULL) return false;

    QMap<QString, StageState> newStageStates;

    QStringList stageList = myType->getStageSequence();
    CaseState currentState = getCaseState();

    if ((currentState == CaseState::DOWNLOAD) ||
            (currentState == CaseState::ERROR) ||
            (currentState == CaseState::INVALID) ||
            (currentState == CaseState::LOADING) ||
            (currentState == CaseState::OFFLINE))
    {
        StageState allStagesState = StageState::ERROR;

        if (currentState == CaseState::DOWNLOAD)
        {
            allStagesState = StageState::DOWNLOADING;
        }
        else if (currentState == CaseState::LOADING)
        {
            allStagesState = StageState::LOADING;
        }
        else if (currentState == CaseState::OFFLINE)
        {
            allStagesState = StageState::OFFLINE;
        }

        for (auto itr = stageList.begin(); itr != stageList.cend(); itr++)
        {
            newStageStates.insert((*itr), allStagesState);
        }

        return updateStageStatesIfNew(&newStageStates);
    }

    for (auto itr = stageList.begin(); itr != stageList.cend(); itr++)
    {
        newStageStates.insert((*itr), StageState::LOADING);
    }

    if ((caseFolder == NULL) || (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED) ||
            (myState == InternalCaseState::STOPPING_JOB) || (myState == InternalCaseState::MAKING_FOLDER) ||
            (myState == InternalCaseState::INIT_PARAM_UPLOAD) || (myState == InternalCaseState::USER_PARAM_UPLOAD) ||
            (myState == InternalCaseState::FOLDER_CHECK_STOPPED_JOB) || (myState == InternalCaseState::COPYING_FOLDER) ||
            (myState == InternalCaseState::WAITING_FOLDER_DEL))
    {
        return updateStageStatesIfNew(&newStageStates);
    }

    if ((myState == InternalCaseState::STARTING_JOB) || (myState == InternalCaseState::RUNNING_JOB_NORECORD))
    {
        if (newStageStates.contains(runningStage))
        {
            newStageStates[runningStage] = StageState::RUNNING;
        }
    }
    else if (myState == InternalCaseState::RUNNING_JOB_YESRECORD)
    {
        QString trueStage = runningJobNode->getParams().value("stage");
        if (newStageStates.contains(trueStage))
        {
            newStageStates[trueStage] = StageState::RUNNING;
        }
    }

    //Check known files for expected result files
    for (auto itr = stageList.begin(); itr != stageList.cend(); itr++)
    {
        if (newStageStates[*itr] != StageState::LOADING)
        {
            continue;
        }

        FileTreeNode * checkNode = caseFolder->getChildNodeWithName(*itr);
        if (checkNode == NULL)
        {
            newStageStates[*itr] = StageState::UNRUN;
        }
        else
        {
            newStageStates[*itr] = StageState::FINISHED;
        }
    }

    for (int i = 0; i < stageList.length(); i++)
    {
        StageState prevState = StageState::FINISHED;
        StageState nextState = StageState::UNRUN;
        if (i != 0)
        {
            prevState = newStageStates[stageList[i-1]];
        }
        if (i + 1 < stageList.length())
        {
            nextState = newStageStates[stageList[i+1]];
        }
        StageState nowState = newStageStates[stageList[i]];

        if (nowState == StageState::UNRUN)
        {
            if ((prevState != StageState::FINISHED) && (prevState != StageState::FINISHED_PREREQ))
            {
                newStageStates[stageList[i]] = StageState::UNREADY;
            }
        }
        else if (nowState == StageState::FINISHED)
        {
            if (nextState == StageState::FINISHED)
            {
                newStageStates[stageList[i]] = StageState::FINISHED_PREREQ;
            }
        }
    }

    return updateStageStatesIfNew(&newStageStates);
}

void CFDcaseInstance::computeParamList()
{
    if (defunct) return;
    if (!caseDataLoaded()) return;

    FileTreeNode * varFile = caseFolder->getChildNodeWithName(caseParamFileName);
    QByteArray * varStore = varFile->getFileBuffer();

    QJsonDocument varDoc = QJsonDocument::fromJson(*varStore);

    if (varDoc.isNull())
    {
        emitNewState(InternalCaseState::ERROR);
        return;
    }

    QJsonObject varsList = varDoc.object().value("vars").toObject();
    storedParamList.clear();
    prospectiveNewParamList.clear();

    for (auto itr = varsList.constBegin(); itr != varsList.constEnd(); itr++)
    {
        storedParamList.insert(itr.key(),(*itr).toString());
    }
}

bool CFDcaseInstance::allListedJobsHaveDetails(QMap<QString, const RemoteJobData * > jobList)
{
    for (auto itr = jobList.begin(); itr != jobList.end(); itr++)
    {
        if (!(*itr)->detailsLoaded())
        {
            return false;
        }
    }
    return true;
}

QMap<QString, const RemoteJobData * > CFDcaseInstance::getRelevantJobs()
{
    QMap<QString, const RemoteJobData *> ret;

    if (caseFolder == NULL)
    {
        return ret;
    }

    QMap<QString, const RemoteJobData *> jobs = cwe_globals::get_CWE_Driver()->getRunningCWEjobs();

    for (auto itr = jobs.begin(); itr != jobs.end(); itr++)
    {
        QString jobID = (*itr)->getID();

        if (!(*itr)->detailsLoaded())
        {
            ret.insert(jobID, (*itr));
            continue;
        }

        QString jobDir = (*itr)->getInputs().value("directory");

        if (caseFolder->fileNameMatches(jobDir))
        {
            ret.insert(jobID, (*itr));
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

void CFDcaseInstance::connectCaseSignals()
{
    //TODO: Some of these should be async
    QObject::connect(cwe_globals::get_job_handle(), SIGNAL(newJobData()),
                     this, SLOT(jobListUpdated()),
                     Qt::QueuedConnection);
    QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileOpDone(RequestState, QString)),
                     this, SLOT(fileTaskDone(RequestState, QString)),
                     Qt::QueuedConnection);
    QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileSystemChange(FileTreeNode *, FileSystemChange)),
                     this, SLOT(underlyingFilesUpdated(FileTreeNode *, FileSystemChange)),
                     Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(haveNewState(CaseState)),
                     this, SLOT(chainedStateTransition()),
                     Qt::QueuedConnection);
}

void CFDcaseInstance::state_CopyingFolder_taskDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::COPYING_FOLDER) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to create new folder for case. Please try again.", "Error");
        return;
    }

    cwe_globals::get_file_handle()->speculateNodeWithName(expectedNewCaseFolder, true);
    caseFolder = cwe_globals::get_file_handle()->getNodeFromName(expectedNewCaseFolder);
    if (caseFolder == NULL)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to find case folder info. Please reset and try again.", "Network Issue");
        return;
    }
    expectedNewCaseFolder.clear();
    QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                     this, SLOT(caseFolderRemoved()));
    QObject::connect(caseFolder, SIGNAL(fileDataChanged(FileTreeNode *)),
                     this, SLOT(underlyingFilesUpdated(FileTreeNode*)));

    enactDataReload();
    emitNewState(InternalCaseState::INIT_DATA_LOAD);
}

void CFDcaseInstance::state_FolderCheckStopped_fileChange_taskDone()
{
    if (myState != InternalCaseState::FOLDER_CHECK_STOPPED_JOB) return;

    if ((caseFolder->getNodeState() == NodeState::FOLDER_KNOWN_CONTENTS_NOT) ||
            (caseFolder->getNodeState() == NodeState::FOLDER_SPECULATE_IDLE))
    {
        cwe_globals::get_file_handle()->enactFolderRefresh(caseFolder);
        return;
    }

    if (caseFolder->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED) return;

    if (runningStage.isEmpty())
    {
        enactDataReload();
        emitNewState(InternalCaseState::RE_DATA_LOAD);
        return;
    }

    FileTreeNode * folderToRemove = caseFolder->getChildNodeWithName(runningStage);
    if (folderToRemove == NULL)
    {
        enactDataReload();
        emitNewState(InternalCaseState::RE_DATA_LOAD);
        return;
    }

    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        return;
    }

    cwe_globals::get_file_handle()->sendDeleteReq(folderToRemove);
    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to contact DesignSafe. Connection may have been lost. Please reset and try again.", "Network Issue");
        return;
    }
    emitNewState(InternalCaseState::WAITING_FOLDER_DEL);
}

void CFDcaseInstance::state_DataLoad_fileChange_jobList()
{
    if ((myState != InternalCaseState::INIT_DATA_LOAD) && (myState != InternalCaseState::RE_DATA_LOAD)) return;

    if (caseDataInvalid())
    {
        emitNewState(InternalCaseState::INVALID);
        return;
    }
    computeCaseType();

    if (!caseDataLoaded())
    {
        enactDataReload();
        return;
    }

    computeParamList();

    QMap<QString, const RemoteJobData *> relevantJobs = getRelevantJobs();
    if (relevantJobs.isEmpty())
    {
        emitNewState(InternalCaseState::READY);
        return;
    }

    if (allListedJobsHaveDetails(relevantJobs))
    {
        if (relevantJobs.size() > 1)
        {
            emitNewState(InternalCaseState::ERROR);
            cwe_globals::displayPopup("CWE job started outside of this program. Case data now corrupted. Please do not start CWE jobs outside of CWE.", "ERROR");
            return;
        }
        runningJobNode = relevantJobs.first();
        emitNewState(InternalCaseState::RUNNING_JOB_YESRECORD);
    }
    else
    {
        emitNewState(InternalCaseState::RE_DATA_LOAD);
    }
}

void CFDcaseInstance::state_InitParam_taskDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::INIT_PARAM_UPLOAD) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to upload parameters to new case. Please check connection try again with new case.", "Network Error");
        return;
    }

    enactDataReload();
    emitNewState(InternalCaseState::INIT_DATA_LOAD);
}

void CFDcaseInstance::state_MakingFolder_taskDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::MAKING_FOLDER) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Error: Unable to create folder for new case. Please check your new folder name and try again.", "Remote Filesystem error");
        return;
    }

    cwe_globals::get_file_handle()->speculateNodeWithName(expectedNewCaseFolder, true);
    caseFolder = cwe_globals::get_file_handle()->getNodeFromName(expectedNewCaseFolder);
    if (caseFolder == NULL)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Error: Internal record of new case folder does not exist. Please contact developer to fix bug.", "Internal ERROR");
        return;
    }
    expectedNewCaseFolder.clear();
    QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                     this, SLOT(caseFolderRemoved()));
    QObject::connect(caseFolder, SIGNAL(fileDataChanged(FileTreeNode *)),
                     this, SLOT(underlyingFilesUpdated(FileTreeNode*)));

    QMap<QString, QString> allVars;
    QByteArray newFile = produceJSONparams(allVars);

    cwe_globals::get_file_handle()->sendUploadBuffReq(caseFolder, newFile, caseParamFileName);

    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to contact DesignSafe. Please wait and try again.", "Network Issue");
        return;
    }
    emitNewState(InternalCaseState::INIT_PARAM_UPLOAD);
}

void CFDcaseInstance::state_Ready_fileChange_jobList()
{
    if (myState != InternalCaseState::READY) return;

    if (!caseDataLoaded())
    {
        enactDataReload();
        emitNewState(InternalCaseState::RE_DATA_LOAD);
    }

    QMap<QString, const RemoteJobData *> relevantJobs = getRelevantJobs();
    if (relevantJobs.isEmpty())
    {
        emitNewState(InternalCaseState::READY);
        return;
    }

    if (!allListedJobsHaveDetails(relevantJobs))
    {
        enactDataReload();
        emitNewState(InternalCaseState::RE_DATA_LOAD);
    }

    if (relevantJobs.size() > 1)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("CWE job started outside of this program. Case data now corrupted. Please do not start CWE jobs outside of CWE.", "ERROR");
        return;
    }
    runningJobNode = relevantJobs.first();
    emitNewState(InternalCaseState::RUNNING_JOB_YESRECORD);
}

void CFDcaseInstance::state_RunningNoRecord_jobList()
{
    if (myState != InternalCaseState::RUNNING_JOB_NORECORD) return;

    const RemoteJobData * aNode = cwe_globals::get_job_handle()->findJobByID(runningID);
    if (aNode == NULL) return;

    if ((aNode->getState() == "FINISHED") ||
            (aNode->getState() == "FAILED"))
    {
        runningJobNode = NULL;
        emitNewState(InternalCaseState::RE_DATA_LOAD);
    }

    if ((aNode != NULL) && aNode->detailsLoaded())
    {
        runningJobNode = aNode;
        emitNewState(InternalCaseState::RUNNING_JOB_YESRECORD);
    }
}

void CFDcaseInstance::state_RunningYesRecord_jobList()
{
    if (myState != InternalCaseState::RUNNING_JOB_YESRECORD) return;

    QMap<QString, const RemoteJobData *> relevantJobs = getRelevantJobs();
    if (!relevantJobs.contains(runningJobNode->getID()))
    {
        runningJobNode = NULL;
        cwe_globals::get_file_handle()->enactFolderRefresh(caseFolder, true);
        enactDataReload();
        emitNewState(InternalCaseState::RE_DATA_LOAD);
        return;
    }
}

void CFDcaseInstance::state_StartingJob_jobInvoked(QString jobID)
{
    if (myState != InternalCaseState::STARTING_JOB) return;

    if (jobID.isEmpty())
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Unable to collect job ID. Please inform developers of this bug.", "ERROR");
        return;
    }

    runningID = jobID;

    cwe_globals::get_job_handle()->demandJobDataRefresh();
    emitNewState(InternalCaseState::RUNNING_JOB_NORECORD);
}

void CFDcaseInstance::state_StoppingJob_jobKilled()
{
    if (myState != InternalCaseState::STOPPING_JOB) return;

    cwe_globals::get_file_handle()->enactFolderRefresh(caseFolder);
    emitNewState(InternalCaseState::FOLDER_CHECK_STOPPED_JOB);
}

void CFDcaseInstance::state_UserParamUpload_taskDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::USER_PARAM_UPLOAD) return;

    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Error: Unable to change parameters. Please reset and try again.", "Remote Filesystem error");
        return;
    }

    FileTreeNode * paramNode = caseFolder->getChildNodeWithName(caseParamFileName);
    if (paramNode != NULL)
    {
        paramNode->setFileBuffer(NULL);
    }
    enactDataReload();
    emitNewState(InternalCaseState::RE_DATA_LOAD);
}

void CFDcaseInstance::state_WaitingFolderDel_taskDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::WAITING_FOLDER_DEL) return;
    if (invokeStatus != RequestState::GOOD)
    {
        emitNewState(InternalCaseState::ERROR);
        cwe_globals::displayPopup("Error: Unable to clean up canceled task. Please reset and try again.", "Remote Filesystem error");
        return;
    }

    cwe_globals::get_file_handle()->enactFolderRefresh(caseFolder, true);
    enactDataReload();
    emitNewState(InternalCaseState::RE_DATA_LOAD);
}

void CFDcaseInstance::state_Download_recursiveOpDone(RequestState invokeStatus)
{
    if (myState != InternalCaseState::DOWNLOAD) return;
    enactDataReload();
    emitNewState(InternalCaseState::RE_DATA_LOAD);
    if (invokeStatus == RequestState::GOOD)
    {
        cwe_globals::displayPopup("Case results successfully downloaded.", "Download Complete");
    }
    else
    {
        cwe_globals::displayPopup("Unable to download case, please check connection and try again.", "Download Error");
    }

}
