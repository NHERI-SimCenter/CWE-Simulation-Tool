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

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/filetreenode.h"
#include "../AgaveExplorer/remoteFileOps/easyboollock.h"
#include "../AgaveExplorer/remoteFileOps/joboperator.h"

#include "../AgaveClientInterface/filemetadata.h"

#include "vwtinterfacedriver.h"

CFDcaseInstance::CFDcaseInstance(FileTreeNode * newCaseFolder, VWTinterfaceDriver * mainDriver):
    QObject((QObject *) mainDriver)
{
    caseFolder = newCaseFolder;
    theDriver = mainDriver;

    QObject::connect(theDriver->getFileHandler(), SIGNAL(newFileInfo()),
                     this, SLOT(underlyingFilesUpdated()));
    QObject::connect(caseFolder, SIGNAL(destroyed(QObject*)),
                     this, SLOT(caseFolderRemoved()));

    myLock = new EasyBoolLock(this);

    underlyingFilesUpdated();
    forceInfoRefresh();
}

CFDcaseInstance::CFDcaseInstance(CFDanalysisType * caseType, VWTinterfaceDriver *mainDriver):
    QObject((QObject *) mainDriver)
{
    myType = caseType;
    theDriver = mainDriver;

    QObject::connect(theDriver->getFileHandler(), SIGNAL(newFileInfo()),
                     this, SLOT(underlyingFilesUpdated()));
}

CFDcaseInstance::~CFDcaseInstance()
{
    if (myLock != NULL)
    {
        delete myLock;
    }
}

bool CFDcaseInstance::isDefunct()
{
    return defunct;
}

CaseState CFDcaseInstance::getCaseState()
{
    if (defunct) return CaseState::DEFUNCT;
    if (myLock->lockClosed()) return CaseState::AGAVE_INVOKE;

    if (caseFolder == NULL) return CaseState::LOADING;
    if (myType == NULL) return CaseState::LOADING;
    if (caseFolder->childIsUnloaded()) return CaseState::LOADING;

    if (caseFolder->getChildNodeWithName(".varStore") == NULL) return CaseState::INVALID;
    if (myType == NULL) return CaseState::INVALID;

    QMap<QString, StageState> stages = getStageStates();
    for (auto itr = stages.cbegin(); itr != stages.cend(); itr++)
    {
        if ((*itr) == StageState::LOADING)
        {
            return CaseState::LOADING;
        }
        if ((*itr) == StageState::ERROR)
        {
            return CaseState::ERROR;
        }
    }
    return CaseState::READY;
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

    //TODO: Return list read from current cache of .varStore
    return ret;
}

QMap<QString, StageState> CFDcaseInstance::getStageStates()
{
    //TODO: check job handler for running tasks on this folder
    //TODO: check known files for expected result files
    //TODO: may return loading if relevant folders are not available
    QMap<QString, StageState> ret;
    if (defunct) return ret;
    ret.insert("mesh", StageState::UNRUN);
    ret.insert("sim", StageState::UNRUN);
    return ret;
}

void CFDcaseInstance::forceInfoRefresh()
{
    if (defunct) return;

    //Enact LS of case folder itself

    //If case type known:
    //Enact LS of folders from list of check files in configuration
    //Enact buffer download of .varStore
}

void CFDcaseInstance::createCase(QString newName, FileTreeNode * containingFolder)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: set expectedNewCaseFolder
    //TODO: invoke agave app to create new case in folder

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
}

void CFDcaseInstance::changeParameters(QMap<QString, QString> paramList)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: Clear the cache of var store

    //TODO: Invoke change paramters agave app

    qDebug("sending ...\n");
    for (auto itr = paramList.cbegin(); itr != paramList.cend(); itr++)
    {
        qDebug("%s: %s", qPrintable(itr.key()), qPrintable(*itr));
    }

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
}

void CFDcaseInstance::mesh(FileTreeNode * geoFile)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: invoke mesh app

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
}

void CFDcaseInstance::rollBack(QStringList stagesToDelete)
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: invoke roll back app

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
}

void CFDcaseInstance::openFOAM()
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: invoke openFOAM app

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
}

void CFDcaseInstance::postProcess()
{
    if (defunct) return;
    if (!myLock->checkAndClaim()) return;
    //TODO: invoke postProcess app

    //Debug:
    QTimer::singleShot(2500, this, SLOT(agaveAppDone()));
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
    //If caseFolder is null, try to find expectedNewCaseFolder as file node
    //If setting file node, connect caseFolderRemoved slot

    //TODO: if caseFolder exists, try to determine myType from brandingFile list
    //Obtain list of CFDanalysis types from VWT driver to get brandingFile list
    //If case type determined, call: forceInfoRefresh() (To get varStore)
    //If type known call getStageStates(), set a 5 second update timer to run forceInfoRefresh() (unless already set)
    emitNewState();
}

void CFDcaseInstance::caseFolderRemoved()
{
    killCaseConnection();
    theDriver->setCurrentCase(NULL);
}

void CFDcaseInstance::agaveAppDone()
{
    if (defunct) return;
    myLock->release();
    underlyingFilesUpdated();
    forceInfoRefresh();
}

void CFDcaseInstance::emitNewState()
{
    if (defunct == true) return;
    CaseState newState = getCaseState();
    if (newState == oldState) return;
    emit haveNewState(oldState, newState);
    oldState = newState;
}
