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

#include "remotefiletree.h"

#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/filemetadata.h"

#include "fileoperator.h"
#include "filetreenode.h"

#include "utilWindows/errorpopup.h"

RemoteFileTree::RemoteFileTree(QObject *parent) :
    QTreeView(parent)
{
    QObject::connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));

    selectedItem = NULL;
}

RemoteFileTree::~RemoteFileTree()
{
    //TODO: Delete entries in the file data tree
}

void RemoteFileTree::setFileOperator(FileOperator * theOperator)
{
    myFileOperator = theOperator;
    myFileOperator->linkToFileTree(this);
}

FileOperator * RemoteFileTree::getFileOperator()
{
    return myFileOperator;
}

void RemoteFileTree::setSelectedLabel(QLabel * selectedFileDisp)
{
    selectedFileDisplay = selectedFileDisp;
}

void RemoteFileTree::resendSelectedFile()
{
    if (linkedFileView->selectionModel()->selectedIndexes().size() <= 0) return;
    fileEntryTouched(linkedFileView->selectionModel()->selectedIndexes().at(0));
}

FileTreeNode * RemoteFileTree::getSelectedNode()
{
    return selectedItem;
}

void RemoteFileTree::resetFileData()
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

void RemoteFileTree::updateFileInfo(QList<FileMetaData> * fileDataList)
{


    translateFileDataToModel();
}

void RemoteFileTree::folderExpanded(QModelIndex itemOpened)
{
    FileTreeNode * theFileNode = getNodeFromModel(dataStore.itemFromIndex(itemOpened));
    fileEntryTouched(itemOpened);

    if (theFileNode == NULL) return;
    if (!theFileNode->childIsUnloaded()) return;

    myFileOperator->enactFolderRefresh(theFileNode->getFileData());
}

void RemoteFileTree::fileEntryTouched(QModelIndex fileIndex)
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

void RemoteFileTree::lsClosestNode(QString fullPath)
{
    FileTreeNode * nodeToRefresh = getDirNearestFromPath(fullPath);
    myFileOperator->enactFolderRefresh(nodeToRefresh->getFileData());
}

void RemoteFileTree::lsClosestNodeToParent(QString fullPath)
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

QString RemoteFileTree::getFilePathForNode(QModelIndex dataIndex)
{
    FileTreeNode * nodeToCheck = getNodeFromModel(dataStore.itemFromIndex(dataIndex));
    if (nodeToCheck == NULL) return "";
    return nodeToCheck->getFileData().getFullPath();
}


QStandardItem * RemoteFileTree::getModelEntryFromNode(FileTreeNode * toFind)
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

void RemoteFileTree::translateFileDataToModel()
{
    FileTreeNode * currentFile = rootFileNode;
    QStandardItem * currentModelEntry = dataStore.invisibleRootItem();

    translateFileDataRecurseHelper(currentFile, currentModelEntry);
}

void RemoteFileTree::translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry)
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

bool RemoteFileTree::fileInModel(FileTreeNode * toFind, QStandardItem * compareTo)
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

void RemoteFileTree::changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource)
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

void RemoteFileTree::newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource)
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

bool RemoteFileTree::columnInUse(int i)
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

QString RemoteFileTree::getRawColumnData(int i, FileMetaData * rawFileData)
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

FileTreeNode * RemoteFileTree::getFileNodeFromPath(QString filePath)
{
    //TODO
}

FileTreeNode * RemoteFileTree::getDirNearestFromPath(QString filePath)
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

FileTreeNode * RemoteFileTree::getNodeFromModel(QStandardItem * toFind)
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
