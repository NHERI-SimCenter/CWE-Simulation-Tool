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

#include "remotefilewindow.h"
#include "ui_remotefilewindow.h"

#include "../AgaveClientInterface/filemetadata.h"

#include "fileoperator.h"
#include "filetreenode.h"
#include "joboperator.h"

#include "vwtinterfacedriver.h"

#include "utilWindows/errorpopup.h"

RemoteFileWindow::RemoteFileWindow(VWTinterfaceDriver * newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RemoteFileWindow)
{
    ui->setupUi(this);

    linkedFileView = this->findChild<QTreeView *>("remoteFileView");
    selectedFileDisplay = this->findChild<QLabel *>("selectedFileInfo");
    QListView * jobList = this->findChild<QListView *>("longTaskView");

    QObject::connect(linkedFileView, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(needRightClickMenuFiles(QPoint)));

    linkedFileView->setModel(&dataStore);

    selectedItem = NULL;

    //Note: setting this as parent should deconstruct the fileOperator
    myFileOperator = new FileOperator(newDriver->getDataConnection(),this);
    myJobOperator = new JobOperator(newDriver->getDataConnection(),jobList,this);
}

RemoteFileWindow::~RemoteFileWindow()
{
    delete ui;

    //TODO: Delete entries in the file data tree
}

void RemoteFileWindow::resendSelectedFile()
{
    if (linkedFileView->selectionModel()->selectedIndexes().size() <= 0) return;
    fileEntryTouched(linkedFileView->selectionModel()->selectedIndexes().at(0));
}

FileMetaData RemoteFileWindow::getCurrentSelectedFile()
{
    if (selectedItem == NULL)
    {
        FileMetaData empty;
        return empty;
    }
    return selectedItem->getFileData();
}

void RemoteFileWindow::resetFileData()
{
    //TODO: reconsider needed columns
    dataStore.clear();
    dataStore.setColumnCount(tableNumCols);
    dataStore.setHorizontalHeaderLabels(shownHeaderLabelList);
    linkedFileView->hideColumn((int)FileColumn::MIME_TYPE);
    linkedFileView->hideColumn((int)FileColumn::PERMISSIONS);
    linkedFileView->hideColumn((int)FileColumn::FORMAT);
    linkedFileView->hideColumn((int)FileColumn::LAST_CHANGED);
    //TODO: Adjust column size defaults;

    if (rootFileNode != NULL)
    {
        rootFileNode->deleteLater();
    }
    selectedItem = NULL;
    rootFileNode = new FileTreeNode();

    new FileTreeNode(rootFileNode);
    translateFileDataToModel();

    myFileOperator->enactFolderRefresh(rootFileNode->getFileData());
}

void RemoteFileWindow::updateFileInfo(QList<FileMetaData> * fileDataList)
{
    QString searchPath;

    for (auto itr = fileDataList->begin(); itr != fileDataList->end(); ++itr)
    {
        if ((*itr).getFileName() == ".")
        {
            searchPath = (*itr).getContainingPath();
            break;
        }
    }

    if (searchPath.isEmpty())
    {
        myFileOperator->totalResetErrorProcedure();
        return;
    }

    FileTreeNode * targetNode = NULL;

    //TODO: if our file data is empty, we need to populate it
    if (rootFileNode->childIsUnloaded())
    {
        QString possiblePath = FileMetaData::cleanPathSlashes(searchPath);
        if (possiblePath.at(0) == '/') possiblePath.remove(0,1);
        if (possiblePath.at(possiblePath.size() - 1) == '/') possiblePath.remove(possiblePath.size() - 1,1);
        if (possiblePath.contains('/'))
        {
            myFileOperator->totalResetErrorProcedure();
            return;
        }
        possiblePath.prepend("/");
        rootFileNode->clearAllChildren();
        FileMetaData userNameRoot;
        userNameRoot.setType(FileType::DIR);
        userNameRoot.setFullFilePath(possiblePath);
        targetNode = new FileTreeNode(userNameRoot, rootFileNode);
        new FileTreeNode(targetNode);
    }
    else
    {
        targetNode = getFileNodeFromPath(searchPath);
    }

    if (targetNode == NULL)
    {
        myFileOperator->totalResetErrorProcedure();
        return;
    }

    //If the target node has a loading placeholder, clear it
    if (targetNode->childIsUnloaded())
    {
        targetNode->clearAllChildren();
    }

    //If the incoming list is empty, ie. has one entry (.), place empty file placeholder
    if (fileDataList->size() <= 1)
    {
        targetNode->clearAllChildren();

        FileMetaData emptyFolder;
        QString emptyName = searchPath;
        emptyName.append("/Empty Folder");
        emptyFolder.setFullFilePath(emptyName);
        emptyFolder.setType(FileType::EMPTY_FOLDER);
        new FileTreeNode(emptyFolder,targetNode);
        translateFileDataToModel();
        return;
    }

    //Unmark all files in the old list
    for (auto itr = targetNode->fileListStart(); itr != targetNode->fileListEnd(); itr++)
    {
        (*itr)->setMark(false);
    }

    //For each file in the new file list, check for it
    //Mark if it is there and IDENTICAL
    //Place new file and mark not found
    for (auto itr = fileDataList->begin(); itr != fileDataList->end(); ++itr)
    {
        if ((*itr).getFileName() == ".") continue;

        bool foundOldFile = false;
        for (auto itr2 = targetNode->fileListStart(); itr2 != targetNode->fileListEnd(); itr2++)
        {
            if ((*itr2)->isMarked()) continue;

            if ((*itr) == (*itr2)->getFileData())
            {
                foundOldFile = true;
                (*itr2)->setMark(true);
                break;
            }
        }
        if (!foundOldFile)
        {
            FileTreeNode * newFile = new FileTreeNode((*itr),targetNode);
            if ((*itr).getFileType() == FileType::DIR)
            {   //If its a new folder, put the loading placeholder in it
                new FileTreeNode(newFile);
            }
            newFile->setMark(true);
        }
    }

    FileTreeNode * toRemove = NULL;
    //Remove all unmarked files from old list -- TODO: This is inelegant code, fix
    do
    {
        toRemove = NULL;
        for (auto itr = targetNode->fileListStart(); itr != targetNode->fileListEnd(); itr++)
        {
            if ((*itr)->isMarked()) continue;

            toRemove = (*itr);
        }
        if (toRemove != NULL)
        {
            delete toRemove;
        }
    }
    while (toRemove != NULL);

    translateFileDataToModel();
}

void RemoteFileWindow::folderExpanded(QModelIndex itemOpened)
{
    FileTreeNode * theFileNode = getNodeFromModel(dataStore.itemFromIndex(itemOpened));
    fileEntryTouched(itemOpened);

    if (theFileNode == NULL) return;
    if (!theFileNode->childIsUnloaded()) return;

    myFileOperator->enactFolderRefresh(theFileNode->getFileData());
}

void RemoteFileWindow::fileEntryTouched(QModelIndex fileIndex)
{
    QStandardItem * touchedItem = dataStore.itemFromIndex(fileIndex);
    selectedItem = getNodeFromModel(touchedItem);

    if (selectedItem == NULL)
    {
        selectedFileDisplay->setText("No File Selected.");
        return;
    }

    FileMetaData newFileData = selectedItem->getFileData();

    QString fileString = "Filename: %1\nType: %2\nSize: %3";
    fileString = fileString.arg(newFileData.getFileName(),
                                newFileData.getFileTypeString(),
                                QString::number(newFileData.getSize()));

    selectedFileDisplay->setText(fileString);

    emit newFileSelected(&newFileData);
}

void RemoteFileWindow::needRightClickMenuFiles(QPoint pos)
{
    QModelIndex targetIndex = linkedFileView->indexAt(pos);
    fileEntryTouched(targetIndex);

    //If we did not click anything, we should return
    if (selectedItem == NULL) return;
    if (selectedItem->isRootNode()) return;
    FileMetaData theFileData = selectedItem->getFileData();

    if (theFileData.getFileType() == FileType::INVALID) return;
    if (theFileData.getFileType() == FileType::UNLOADED) return;
    if (theFileData.getFileType() == FileType::EMPTY_FOLDER) return;

    QMenu fileMenu;

    if (myFileOperator->operationIsPending())
    {
        fileMenu.addAction("File Operation In Progress . . .");
        fileMenu.exec(QCursor::pos());
        return;
    }

    fileMenu.addAction("Copy To . . .",myFileOperator, SLOT(sendCopyReq()));
    fileMenu.addAction("Move To . . .",myFileOperator, SLOT(sendMoveReq()));
    fileMenu.addAction("Rename",myFileOperator, SLOT(sendRenameReq()));
    //We don't let the user delete the username folder
    if (!(selectedItem->getParentNode()->isRootNode()))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Delete",myFileOperator, SLOT(sendDeleteReq()));
        fileMenu.addSeparator();
    }
    if (theFileData.getFileType() == FileType::DIR)
    {
        fileMenu.addAction("Upload File Here",myFileOperator, SLOT(sendUploadReq()));
        fileMenu.addAction("Create New Folder",myFileOperator, SLOT(sendCreateFolderReq()));
    }
    if (theFileData.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("Download File",myFileOperator, SLOT(sendDownloadReq()));
    }
    if (theFileData.getFileType() == FileType::DIR)
    {
        fileMenu.addAction("Compress Folder",myFileOperator, SLOT(sendCompressReq()));
    }
    else if (theFileData.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("De-Compress File",myFileOperator, SLOT(sendDecompressReq()));
    }

    if ((theFileData.getFileType() == FileType::DIR) || (theFileData.getFileType() == FileType::FILE))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Refresh Data",myFileOperator, SLOT(sendManualRefresh()));
        fileMenu.addSeparator();
    }

    fileMenu.exec(QCursor::pos());
}

void RemoteFileWindow::lsClosestNode(QString fullPath)
{
    FileTreeNode * nodeToRefresh = getDirNearestFromPath(fullPath);
    myFileOperator->enactFolderRefresh(nodeToRefresh->getFileData());
}

void RemoteFileWindow::lsClosestNodeToParent(QString fullPath)
{
    FileTreeNode * nodeToRefresh = getFileNodeFromPath(fullPath);
    if (nodeToRefresh != NULL)
    {
        if (!nodeToRefresh->isRootNode())
        {
            nodeToRefresh = nodeToRefresh->getParentNode();
        }
        myFileOperator->enactFolderRefresh(nodeToRefresh->getFileData());
        return;
    }

    nodeToRefresh = getDirNearestFromPath(fullPath);
    myFileOperator->enactFolderRefresh(nodeToRefresh->getFileData());
}

QString RemoteFileWindow::getFilePathForNode(QModelIndex dataIndex)
{
    FileTreeNode * nodeToCheck = getNodeFromModel(dataStore.itemFromIndex(dataIndex));
    if (nodeToCheck == NULL) return "";
    return nodeToCheck->getFileData().getFullPath();
}


QStandardItem * RemoteFileWindow::getModelEntryFromNode(FileTreeNode * toFind)
{
    if (toFind == NULL) return NULL;

    QStandardItem * searchPointer = dataStore.invisibleRootItem();

    QStringList pathSearchList = FileMetaData::getPathNameList(toFind->getFileData().getFullPath());

    for (auto itr = pathSearchList.cbegin(); itr != pathSearchList.cend(); itr++)
    {
        bool foundNext = false;
        for (int i = 0; i < searchPointer->rowCount(); i++)
        {
            if (searchPointer->child(i,(int)FileColumn::FILENAME)->text() == (*itr))
            {
                searchPointer = searchPointer->child(i,(int)FileColumn::FILENAME);
                foundNext = true;
                break;
            }
        }
        if (foundNext == false)
        {
            return NULL;
        }
    }

    if (fileInModel(toFind,searchPointer))
    {
        return searchPointer;
    }
    return NULL;
}

void RemoteFileWindow::translateFileDataToModel()
{
    FileTreeNode * currentFile = rootFileNode;
    QStandardItem * currentModelEntry = dataStore.invisibleRootItem();

    translateFileDataRecurseHelper(currentFile, currentModelEntry);
}

void RemoteFileWindow::translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry)
{
    //TODO: I am guessing this could be more efficient
    for (auto itr = currentFile->fileListStart(); itr != currentFile->fileListEnd(); itr++)
    {
        (*itr)->setMark(false);
    }

    for (int i = 0; i < currentModelEntry->rowCount(); i++)
    {
        QStandardItem * testItem = currentModelEntry->child(i, (int)FileColumn::FILENAME);
        FileTreeNode * testFile = currentFile->getChildNodeWithName(testItem->text());
        if (testFile == NULL)
        {
            currentModelEntry->removeRow(i);
            i = 0;
        }
        else
        {
            changeModelFromFile(testItem,testFile);
            testFile->setMark(true);
        }
    }

    for (auto itr = currentFile->fileListStart(); itr != currentFile->fileListEnd(); itr++)
    {
        if ((*itr)->isMarked() == false)
        {
            newModelRowFromFile(currentModelEntry,(*itr));
        }
    }

    for (int i = 0; i < currentModelEntry->rowCount(); i++)
    {
        QStandardItem * testItem = currentModelEntry->child(i, (int)FileColumn::FILENAME);
        FileTreeNode * testFile = currentFile->getChildNodeWithName(testItem->text(), true);
        if (testFile == NULL)
        {
            ErrorPopup("Internal file tree parse is self-inconsistant.");
            return;
        }
        translateFileDataRecurseHelper(testFile,testItem);
    }
}

bool RemoteFileWindow::fileInModel(FileTreeNode * toFind, QStandardItem * compareTo)
{
    if ((toFind == NULL) || (compareTo == NULL))
    {
        return false;
    }
    FileMetaData rawData = toFind->getFileData();
    QStandardItem * parentNode = compareTo->parent();
    if (parentNode == NULL)
    {
        parentNode = dataStore.invisibleRootItem();
    }

    int rowNum = compareTo->row();
    for (int i = 0; i < tableNumCols; i++)
    {
        if (columnInUse(i))
        {
            if (parentNode->child(rowNum,i)->text() != getRawColumnData(i,&rawData))
            {
                return false;
            }
        }
    }
    return true;
}

void RemoteFileWindow::changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource)
{
    if ((targetRow == NULL) || (dataSource == NULL))
    {
        ErrorPopup("NULL pointer in changeModelFromFile method");
        return;
    }

    FileMetaData rawData = dataSource->getFileData();
    QStandardItem * parentNode = targetRow->parent();
    if (parentNode == NULL)
    {
        parentNode = dataStore.invisibleRootItem();
    }

    int rowNum = targetRow->row();
    for (int i = 0; i < tableNumCols; i++)
    {
        QStandardItem * valToSwitch = parentNode->child(rowNum,i);

        if (columnInUse(i))
        {
            valToSwitch->setText(getRawColumnData(i,&rawData));
        }
    }
}

void RemoteFileWindow::newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource)
{
    if ((parentItem == NULL) || (dataSource == NULL))
    {
        ErrorPopup("NULL pointer in changeModelFromFile method");
        return;
    }
    FileMetaData rawData = dataSource->getFileData();
    QList<QStandardItem *> newDataList;

    for (int i = 0; i < tableNumCols; i++)
    {
        if (columnInUse(i))
        {
            newDataList.append(new QStandardItem(getRawColumnData(i,&rawData)));
        }
        else
        {
            newDataList.append(new QStandardItem(""));
        }
    }

    parentItem->appendRow(newDataList);
}

bool RemoteFileWindow::columnInUse(int i)
{
    //TODO: This is a temporary function until the used/hidden columns are clarified
    if ((FileColumn)i == FileColumn::FILENAME)
    {
        return true;
    }
    else if ((FileColumn)i == FileColumn::TYPE)
    {
        return true;
    }
    else if ((FileColumn)i == FileColumn::SIZE)
    {
        return true;
    }
    return false;
}

QString RemoteFileWindow::getRawColumnData(int i, FileMetaData * rawFileData)
{
    if ((FileColumn)i == FileColumn::FILENAME)
    {
        return rawFileData->getFileName();
    }
    else if ((FileColumn)i == FileColumn::TYPE)
    {
        return rawFileData->getFileTypeString();
    }
    else if ((FileColumn)i == FileColumn::SIZE)
    {
        return QString::number(rawFileData->getSize());
    }
    return "";
}


FileTreeNode * RemoteFileWindow::getFileNodeFromPath(QString filePath)
{
    QStringList filePathParts = FileMetaData::getPathNameList(filePath);
    FileTreeNode * searchNode = rootFileNode;

    for (auto itr = filePathParts.cbegin(); itr != filePathParts.cend(); itr++)
    {
        FileTreeNode * nextNode = searchNode->getChildNodeWithName(*itr);
        if (nextNode == NULL)
        {
            return NULL;
        }
        searchNode = nextNode;
    }

    return searchNode;
}

FileTreeNode * RemoteFileWindow::getDirNearestFromPath(QString filePath)
{
    QStringList filePathParts = FileMetaData::getPathNameList(filePath);
    FileTreeNode * searchNode = rootFileNode;

    for (auto itr = filePathParts.cbegin(); itr != filePathParts.cend(); itr++)
    {
        FileTreeNode * nextNode = searchNode->getChildNodeWithName(*itr);
        if ((nextNode == NULL) || (nextNode->getFileData().getFileType() != FileType::DIR))
        {
            return searchNode;
        }
        searchNode = nextNode;
    }

    return searchNode;
}

FileTreeNode * RemoteFileWindow::getNodeFromModel(QStandardItem * toFind)
{
    if (toFind == NULL)
    {
        return NULL;
    }

    if (toFind->column() != (int)FileColumn::FILENAME)
    {
        QStandardItem * parentItem = toFind->parent();
        if (parentItem == NULL)
        {
            parentItem = dataStore.invisibleRootItem();
        }
        toFind = parentItem->child(toFind->row(),(int)FileColumn::FILENAME);
    }

    QString pathToFind;

    QStandardItem * searchPointer = toFind;

    while (searchPointer != NULL)
    {
        pathToFind.prepend(searchPointer->text());
        pathToFind.prepend("/");
        searchPointer = searchPointer->parent();
    }

    return getFileNodeFromPath(pathToFind);
}
