/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
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

#include "debugpanelwindow.h"
#include "ui_debugpanelwindow.h"

#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/filemetadata.h"

#include "remoteFileOps/filetreenode.h"
#include "remoteFileOps/fileoperator.h"
#include "remoteFileOps/remotefiletree.h"
#include "remoteFileOps/joboperator.h"

#include "visualUtils/decompresswrapper.h"

#include "utilWindows/singlelinedialog.h"
#include "utilWindows/deleteconfirm.h"

DebugPanelWindow::DebugPanelWindow(RemoteDataInterface *newDataLink, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugPanelWindow)
{
    ui->setupUi(this);

    dataLink = newDataLink;

    agaveParamLists.insert("FileEcho",{"NewFile", "EchoText"});
    agaveParamLists.insert("PythonTest",{"NewFile"});
    agaveParamLists.insert("SectionMesh",{"directory", "SlicePlane","SimParams"});

    agaveParamLists.insert("compress",{"directory", "compression_type"});
    agaveParamLists.insert("extract", {"inputFile"});
    agaveParamLists.insert("openfoam", {"solver","inputDirectory"});
    agaveParamLists.insert("tempCFD",{"solver","inputDirectory"});

    agaveParamLists.insert("twoDslice", {"SlicePlane", "SimParams", "NewCaseFolder","SGFFile"});
    agaveParamLists.insert("twoDUmesh", {"MeshParams","directory"});

    for (auto itr = agaveParamLists.keyBegin(); itr != agaveParamLists.keyEnd(); itr++)
    {
        taskListModel.appendRow(new QStandardItem(*itr));
    }
    ui->agaveAppList->setModel(&taskListModel);
}

DebugPanelWindow::~DebugPanelWindow()
{
    delete ui;
}

void DebugPanelWindow::startAndShow()
{
    theFileOperator = new FileOperator(dataLink, this);

    theFileOperator->resetFileData();
    ui->remoteFileView->setFileOperator(theFileOperator);
    ui->remoteFileView->setupFileView();
    QObject::connect(ui->remoteFileView, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(customFileMenu(QPoint)));

    ui->agaveAppList->setModel(&taskListModel);
    ui->agaveAppList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->show();
}

void DebugPanelWindow::agaveAppSelected(QModelIndex clickedItem)
{
    QString newSelection = taskListModel.itemFromIndex(clickedItem)->text();
    if (selectedAgaveApp == newSelection)
    {
        return;
    }
    selectedAgaveApp = newSelection;

    QGridLayout * panelLayout = new QGridLayout();
    QObjectList childList = ui->AgaveParamWidget->children();

    while (childList.size() > 0)
    {
        QObject * aChild = childList.takeLast();
        delete aChild;
    }

    ui->AgaveParamWidget->setLayout(panelLayout);

    QStringList inputList = agaveParamLists.value(selectedAgaveApp);
    int rowNum = 0;

    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QString paramName = "debugAgave_";
        paramName = paramName.append(*itr);

        QLabel * tmpLabel = new QLabel(*itr);
        QLineEdit * tmpInput = new QLineEdit();
        tmpInput->setObjectName(paramName);

        panelLayout->addWidget(tmpLabel,rowNum,0);
        panelLayout->addWidget(tmpInput,rowNum,1);
        rowNum++;
    }
}

void DebugPanelWindow::setTestVisual()
{
    conditionalPurge(&pointData);
    conditionalPurge(&faceData);
    conditionalPurge(&ownerData);
    ui->openGLcfdWidget->setDisplayState(CFDDisplayState::TEST_BOX);
}

void DebugPanelWindow::setMeshVisual()
{
    /*
    setTestVisual();
    FileTreeNode * currentNode = fileTreeData->getFileNodeFromPath(selectedFullPath);

    FileTreeNode * constantFolder = currentNode->getChildNodeWithName("constant");
    if (constantFolder == NULL) return;
    FileTreeNode * polyMeshFolder = constantFolder->getChildNodeWithName("polyMesh");
    if (polyMeshFolder == NULL) return;
    FileTreeNode * pointsFile = polyMeshFolder->getChildNodeWithName("points");
    FileTreeNode * facesFile = polyMeshFolder->getChildNodeWithName("faces");
    FileTreeNode * ownerFile = polyMeshFolder->getChildNodeWithName("owner");
    if (pointsFile == NULL) pointsFile = polyMeshFolder->getChildNodeWithName("points.gz");
    if (facesFile == NULL) facesFile = polyMeshFolder->getChildNodeWithName("faces.gz");
    if (ownerFile == NULL) ownerFile = polyMeshFolder->getChildNodeWithName("owner.gz");

    if ((pointsFile == NULL) || (facesFile == NULL) || (ownerFile == NULL)) return;

    RemoteDataReply * aReply = dataLink->downloadBuffer(pointsFile->getFileData().getFullPath());
    QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                     this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

    aReply = dataLink->downloadBuffer(facesFile->getFileData().getFullPath());
    QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                     this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

    aReply = dataLink->downloadBuffer(ownerFile->getFileData().getFullPath());
    QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                     this,SLOT(gotNewRawFile(RequestState,QByteArray*)));
                     */
}

void DebugPanelWindow::gotNewRawFile(RequestState authReply, QByteArray * fileBuffer)
{
    /*
    if (authReply != RequestState::GOOD) return;

    RemoteDataReply * mySender = (RemoteDataReply *) QObject::sender();
    if (mySender == NULL) return;
    QString lookedForFile = mySender->getTaskParamList()->value("remoteName");
    if (lookedForFile.isEmpty()) return;

    QByteArray * realContents;

    if (lookedForFile.endsWith(".gz"))
    {
        lookedForFile.chop(3);
        DeCompressWrapper decompresser(fileBuffer);
        realContents = decompresser.getDecompressedFile();
    }
    else
    {
        realContents = new QByteArray(*fileBuffer);
    }
    if (lookedForFile.endsWith("points"))
    {
        conditionalPurge(&pointData);
        pointData = realContents;
    }
    if (lookedForFile.endsWith("faces"))
    {
        conditionalPurge(&faceData);
        faceData = realContents;
    }
    if (lookedForFile.endsWith("owner"))
    {
        conditionalPurge(&ownerData);
        ownerData = realContents;
    }
    if ((pointData != NULL) && (faceData != NULL) && (ownerData != NULL))
    {
        if(ui->openGLcfdWidget->loadMeshData(pointData, faceData, ownerData))
        {
            ui->openGLcfdWidget->setDisplayState(CFDDisplayState::MESH);
        }
    }
    */
}

void DebugPanelWindow::agaveCommandInvoked()
{
    if (waitingOnCommand)
    {
        return;
    }
    qDebug("Selected App: %s", qPrintable(selectedAgaveApp));

    QString workingDir = ui->remoteFileView->getSelectedNode()->getFileData().getFullPath();
    qDebug("Working Dir: %s", qPrintable(workingDir));

    QStringList inputList = agaveParamLists.value(selectedAgaveApp);
    QMultiMap<QString, QString> allInputs;

    qDebug("Input List:");
    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QString paramName = "debugAgave_";
        paramName = paramName.append(*itr);

        QLineEdit * theInput = ui->AgaveParamWidget->findChild<QLineEdit *>(paramName);
        if (theInput != NULL)
        {
            allInputs.insert((*itr),theInput->text());
            qDebug("%s : %s", qPrintable(*itr), qPrintable(theInput->text()));
        }
    }

    RemoteDataReply * theTask = dataLink->runRemoteJob(selectedAgaveApp,allInputs,workingDir);
    if (theTask == NULL)
    {
        qDebug("Unable to invoke task");
        return;
    }
    waitingOnCommand = true;
    QObject::connect(theTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(finishedAppInvoke(RequestState,QJsonDocument*)));
}

void DebugPanelWindow::finishedAppInvoke(RequestState, QJsonDocument *)
{
    waitingOnCommand = false;
}

void DebugPanelWindow::customFileMenu(QPoint pos)
{
    QMenu fileMenu;
    if (ui->remoteFileView->getFileOperator()->operationIsPending())
    {
        fileMenu.addAction("File Operation In Progress . . .");
        fileMenu.exec(QCursor::pos());
        return;
    }

    QModelIndex targetIndex = ui->remoteFileView->indexAt(pos);
    ui->remoteFileView->fileEntryTouched(targetIndex);

    targetNode = ui->remoteFileView->getSelectedNode();

    //If we did not click anything, we should return
    if (targetNode == NULL) return;
    if (targetNode->isRootNode()) return;
    FileMetaData theFileData = targetNode->getFileData();

    if (theFileData.getFileType() == FileType::INVALID) return;
    if (theFileData.getFileType() == FileType::UNLOADED) return;
    if (theFileData.getFileType() == FileType::EMPTY_FOLDER) return;

    fileMenu.addAction("Copy To . . .",this, SLOT(copyMenuItem()));
    fileMenu.addAction("Move To . . .",this, SLOT(moveMenuItem()));
    fileMenu.addAction("Rename",this, SLOT(renameMenuItem()));
    //We don't let the user delete the username folder
    if (!(targetNode->getParentNode()->isRootNode()))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Delete",this, SLOT(deleteMenuItem()));
        fileMenu.addSeparator();
    }
    if (theFileData.getFileType() == FileType::DIR)
    {
        fileMenu.addAction("Upload File Here",this, SLOT(uploadMenuItem()));
        fileMenu.addAction("Create New Folder",this, SLOT(createFolderMenuItem()));
    }
    if (theFileData.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("Download File",this, SLOT(downloadMenuItem()));
    }
    if (theFileData.getFileType() == FileType::DIR)
    {
        fileMenu.addAction("Compress Folder",this, SLOT(compressMenuItem()));
    }
    else if (theFileData.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("De-Compress File",this, SLOT(decompressMenuItem()));
    }

    if ((theFileData.getFileType() == FileType::DIR) || (theFileData.getFileType() == FileType::FILE))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Refresh Data",this, SLOT(refreshMenuItem()));
        fileMenu.addSeparator();
    }

    fileMenu.exec(QCursor::pos());
}

void DebugPanelWindow::copyMenuItem()
{
    SingleLineDialog newNamePopup("Please type a file name to copy to:", "newname");
    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    ui->remoteFileView->getFileOperator()->sendCopyReq(targetNode, newNamePopup.getInputText());
}

void DebugPanelWindow::moveMenuItem()
{
    SingleLineDialog newNamePopup("Please type a file name to move to:", "newname");

    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    ui->remoteFileView->getFileOperator()->sendMoveReq(targetNode,newNamePopup.getInputText());
}

void DebugPanelWindow::renameMenuItem()
{
    SingleLineDialog newNamePopup("Please type a new file name:", "newname");

    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    ui->remoteFileView->getFileOperator()->sendRenameReq(targetNode, newNamePopup.getInputText());
}

void DebugPanelWindow::deleteMenuItem()
{
    DeleteConfirm deletePopup(targetNode->getFileData().getFullPath());
    if (deletePopup.exec() != QDialog::Accepted)
    {
        return;
    }
    ui->remoteFileView->getFileOperator()->sendDeleteReq(targetNode);
}

void DebugPanelWindow::uploadMenuItem()
{
    SingleLineDialog uploadNamePopup("Please input full path of file to upload:", "");

    if (uploadNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }
    ui->remoteFileView->getFileOperator()->sendUploadReq(targetNode, uploadNamePopup.getInputText());
}

void DebugPanelWindow::createFolderMenuItem()
{
    SingleLineDialog newFolderNamePopup("Please input a name for the new folder:", "newFolder1");

    if (newFolderNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }
    ui->remoteFileView->getFileOperator()->sendCreateFolderReq(targetNode, newFolderNamePopup.getInputText());
}

void DebugPanelWindow::downloadMenuItem()
{
    SingleLineDialog downloadNamePopup("Please input full path download destination:", "");

    if (downloadNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }
    ui->remoteFileView->getFileOperator()->sendDownloadReq(targetNode, downloadNamePopup.getInputText());
}

void DebugPanelWindow::compressMenuItem()
{
    ui->remoteFileView->getFileOperator()->sendCompressReq(targetNode);
}

void DebugPanelWindow::decompressMenuItem()
{
    ui->remoteFileView->getFileOperator()->sendDecompressReq(targetNode);
}

void DebugPanelWindow::refreshMenuItem()
{
    ui->remoteFileView->getFileOperator()->enactFolderRefresh(targetNode);
}

void DebugPanelWindow::conditionalPurge(QByteArray ** theArray)
{
    if (*theArray == NULL) return;
    delete *theArray;
    *theArray = NULL;
}
