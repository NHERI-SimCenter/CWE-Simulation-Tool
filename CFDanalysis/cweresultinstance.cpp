/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#include "cweresultinstance.h"

#include "visualUtils/resultVisuals/resulttextdisp.h"
#include "visualUtils/resultVisuals/resultfield2dwindow.h"
#include "visualUtils/resultVisuals/resultmesh3dwindow.h"
#include "visualUtils/resultVisuals/resultmesh2dwindow.h"

#include "remoteFiles/filetreenode.h"
#include "remoteFiles/fileoperator.h"
#include "cwe_guiWidgets/cwe_results.h"
#include "cweanalysistype.h"
#include "cwe_globals.h"

cweResultInstance::cweResultInstance(QString stageName, RESULT_ENTRY resultData, CWE_Results *parent) : QObject(parent)
{
    myStage = stageName;
    myResultData = resultData;
    myParent = parent;

    QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileSystemChange(FileNodeRef)),
                     this, SLOT(recomputeResultState()), Qt::QueuedConnection);

    if (myResultData.type == "text")
    {
        setInternalParams(true,true,"Data File");
    }
    else if (myResultData.type == "GLdata")
    {
        setInternalParams(true,false,"Flow Field Image");
    }
    else if (myResultData.type == "GLmesh")
    {
        setInternalParams(true,false,"Mesh Image");
    }
    else if (myResultData.type == "GLmesh3D")
    {
        setInternalParams(true,false,"3D Mesh Image");
    }
    else if (myResultData.type == "download")
    {
        setInternalParams(false,true,"Data Download");
    }
}

void cweResultInstance::enactShowOp()
{
    if (myState != ResultInstanceState::LOADED) return;
    CWEcaseInstance * currentCase = myParent->getCurrentCase();
    if (currentCase == nullptr) return;

    if (myResultData.type == "text")
    {
        ResultTextDisplay * resultPopup = new ResultTextDisplay(currentCase, &myResultData, nullptr);
        resultPopup->initializeView();
    }
    else if (myResultData.type == "GLdata")
    {
        ResultField2dWindow * resultPopup = new ResultField2dWindow(currentCase, &myResultData, nullptr);
        resultPopup->initializeView();
    }
    else if (myResultData.type == "GLmesh")
    {
        ResultMesh2dWindow * resultPopup = new ResultMesh2dWindow(currentCase, &myResultData, nullptr);
        resultPopup->initializeView();
    }
    else if (myResultData.type == "GLmesh3D")
    {
        ResultMesh3dWindow * resultPopup = new ResultMesh3dWindow(currentCase, &myResultData, nullptr);
        resultPopup->initializeView();
    }
}

void cweResultInstance::enactDownloadOp()
{
    if (myState != ResultInstanceState::LOADED) return;

    if ((myResultData.type == "text") || (myResultData.type == "download"))
    {
        if (myResultData.file.isEmpty()) return;
        QString localfileName = QFileDialog::getSaveFileName(myParent, "Save Downloaded File:");
        if (localfileName.isEmpty()) return;

        FileNodeRef targetNode = myParent->getCaseFolder().getChildWithName(myStage);
        if (targetNode.isNil())
        {
            cwe_globals::displayPopup("The stage for this download has not been completed. Please check your case and try again.");
            return;
        }

        targetNode = cwe_globals::get_file_handle()->speculateFileWithName(targetNode, myResultData.file, false);
        if (targetNode.isNil())
        {
            cwe_globals::displayPopup("The result to be downloaded does not exist. Please check your case and try again.");
            return;
        }
        cwe_globals::get_file_handle()->sendDownloadReq(targetNode, localfileName);
    }
}

QList<QStandardItem *> cweResultInstance::getItemList()
{
    QList<QStandardItem *> ret;

    QStandardItem *var1 = new QStandardItem(myResultData.displayName);
    ret.append(var1);

    QStandardItem *var2 = new QStandardItem;
    if (showEye) {
        var2->setIcon(QIcon(":/buttons/images/CWE_eye.png"));
    }
    else {
        var2->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    ret.append(var2);

    QStandardItem *var3 = new QStandardItem;
    if (downloadable) {
        var3->setIcon(QIcon(":/buttons/images/CWE_download.png"));
    }
    else {
        var3->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    ret.append(var3);

    QStandardItem *var4 = new QStandardItem(typeString);
    ret.append(var4);

    return ret;
}

ResultInstanceState cweResultInstance::getState()
{
    return myState;
}

void cweResultInstance::recomputeResultState()
{
    if (!myParent->getCaseFolder().folderContentsLoaded())
    {
        changeMyState(ResultInstanceState::UNLOADED);
        return;
    }

    FileNodeRef baseNode = myParent->getCaseFolder().getChildWithName(myStage);
    if (baseNode.isNil())
    {
        changeMyState(ResultInstanceState::NIL);
        return;
    }

    if ((myResultData.type == "text") || (myResultData.type == "download"))
    {
        NodeState fileNodeState = cwe_globals::get_file_handle()->speculateFileWithName(baseNode, myResultData.file, false, false).getNodeState();
        if ((fileNodeState == NodeState::NON_EXTANT) || (fileNodeState == NodeState::ERROR))
        {
            changeMyState(ResultInstanceState::NIL);
            return;
        }
        else if ((fileNodeState == NodeState::FILE_SPECULATE_IDLE) || (fileNodeState == NodeState::FILE_SPECULATE_LOADING))
        {
            changeMyState(ResultInstanceState::UNLOADED);
            return;
        }
        else
        {
            changeMyState(ResultInstanceState::LOADED);
            return;
        }
    }
    else
    {
        if (myResultData.type == "GLdata")
        {
            if (!baseFolderContainsNumber())
            {
                changeMyState(ResultInstanceState::NIL);
                return;
            }
        }

        QList<QString> neededFiles;
        neededFiles.append("/constant/polyMesh/points");
        neededFiles.append("/constant/polyMesh/faces");
        neededFiles.append("/constant/polyMesh/owner");

        for (QString aFileName : neededFiles)
        {
            NodeState fileNodeState1 = cwe_globals::get_file_handle()->speculateFileWithName(baseNode, aFileName, false, false).getNodeState();
            NodeState fileNodeState2 = cwe_globals::get_file_handle()->speculateFileWithName(baseNode, aFileName.append(".gz"), false, false).getNodeState();

            if ( ((fileNodeState1 == NodeState::NON_EXTANT) || (fileNodeState1 == NodeState::ERROR)) &&
                ((fileNodeState2 == NodeState::NON_EXTANT) || (fileNodeState2 == NodeState::ERROR)) )
            {
                changeMyState(ResultInstanceState::NIL);
            }
            else if ( ((fileNodeState1 == NodeState::FILE_SPECULATE_IDLE) || (fileNodeState1 == NodeState::FILE_SPECULATE_LOADING)) &&
                      ((fileNodeState2 == NodeState::FILE_SPECULATE_IDLE) || (fileNodeState2 == NodeState::FILE_SPECULATE_LOADING)) )
            {
                changeMyState(ResultInstanceState::UNLOADED);
                return;
            }
            else
            {
                changeMyState(ResultInstanceState::LOADED);
                return;
            }
        }
    }
}

void cweResultInstance::setInternalParams(bool show, bool download, QString type)
{
    showEye = show;
    downloadable = download;
    typeString = type;
}

void cweResultInstance::changeMyState(ResultInstanceState newState)
{
    if (myState == newState) return;

    myState = newState;
    myParent->changeVisibleState(this, myState);
}

bool cweResultInstance::baseFolderContainsNumber()
{
    for (FileNodeRef childNode : myParent->getCaseFolder().getChildWithName(myStage).getChildList())
    {
        if (childNode.getFileType() != FileType::DIR) continue;

        QString childName = childNode.getFileName();
        if (childName == "0") continue;

        bool isNum = false;
        childName.toDouble(&isNum);
        if (isNum) return true;
    }
    return false;
}
