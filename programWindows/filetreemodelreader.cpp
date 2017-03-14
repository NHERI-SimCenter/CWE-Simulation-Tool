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

#include "filetreemodelreader.h"

#include "../vwtinterfacedriver.h"
#include "../remotedatainterface.h"

#include "../programWindows/deleteconfirm.h"
#include "../programWindows/singlelinedialog.h"
#include "../programWindows/errorpopup.h"

FileTreeModelReader::FileTreeModelReader(RemoteDataInterface * newDataLink, QTreeView * newLinkedFileView, QLabel * fileHeaderLabel)
{
    linkedFileView = newLinkedFileView;
    dataLink = newDataLink;
    filesLabel = fileHeaderLabel;
    QObject::connect(linkedFileView, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(needRightClickMenu(QPoint)));

    linkedFileView->setModel(&dataStore);

    selectedItem = NULL;
}

void FileTreeModelReader::setTreeViewVisibility(bool isVisible)
{
    linkedFileView->setVisible(isVisible);
    filesLabel->setVisible(isVisible);
}

void FileTreeModelReader::setRightClickMenuEnabled(bool newSetting)
{
    rightClickEnabled = newSetting;
}

void FileTreeModelReader::resendSelectedFile()
{
    if (linkedFileView->selectionModel()->selectedIndexes().size() <= 0) return;
    fileEntryTouched(linkedFileView->selectionModel()->selectedIndexes().at(0));
}

FileMetaData FileTreeModelReader::getCurrentSelectedFile()
{
    return selectedItem->getFileData();
}

void FileTreeModelReader::resetFileData()
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

    enactFolderRefresh(rootFileNode);
}

void FileTreeModelReader::folderExpanded(QModelIndex itemOpened)
{
    FileTreeNode * theFileNode = getNodeFromModel(dataStore.itemFromIndex(itemOpened));

    if (theFileNode == NULL) return;
    if (!theFileNode->childIsUnloaded()) return;

    enactFolderRefresh(theFileNode);
}

void FileTreeModelReader::fileEntryTouched(QModelIndex fileIndex)
{
    QStandardItem * touchedItem = dataStore.itemFromIndex(fileIndex);
    selectedItem = getNodeFromModel(touchedItem);

    if (selectedItem == NULL)
    {
        return;
    }

    FileMetaData newFileData = selectedItem->getFileData();
    emit newFileSelected(&newFileData);
}

void FileTreeModelReader::needRightClickMenu(QPoint pos)
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

    if (fileOperationPending)
    {
        fileMenu.addAction("File Operation In Progress . . .");
        fileMenu.exec(QCursor::pos());
        return;
    }

    fileMenu.addAction("Copy To . . .",this, SLOT(sendCopyReq()));
    fileMenu.addAction("Move To . . .",this, SLOT(sendMoveReq()));
    fileMenu.addAction("Rename",this, SLOT(sendRenameReq()));
    //We don't let the user delete the username folder
    if (!(selectedItem->getParentNode()->isRootNode()))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Delete",this, SLOT(sendDeleteReq()));
        fileMenu.addSeparator();
    }
    if (theFileData.getFileType() == FileType::DIR)
    {
        fileMenu.addAction("Upload File Here",this, SLOT(sendUploadReq()));
        fileMenu.addAction("Create New Folder",this, SLOT(sendCreateFolderReq()));
    }
    if (theFileData.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("Download File",this, SLOT(sendDownloadReq()));
    }
    if ((theFileData.getFileType() == FileType::DIR) || (theFileData.getFileType() == FileType::FILE))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Refresh Data",this, SLOT(sendManualRefresh()));
        fileMenu.addSeparator();
    }

    fileMenu.exec(QCursor::pos());
}

void FileTreeModelReader::getLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList)
{
    if (cmdReply != RequestState::GOOD)
    {
        totalResetErrorProcedure();
        return;
    }
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
        totalResetErrorProcedure();
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
            totalResetErrorProcedure();
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
        totalResetErrorProcedure();
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

    //Remove all unmarked files from old list
    for (auto itr = targetNode->fileListStart(); itr != targetNode->fileListEnd(); itr++)
    {
        if ((*itr)->isMarked()) continue;

        //TODO: double check safety of this delete
        delete (*itr);
    }

    translateFileDataToModel();
}

void FileTreeModelReader::sendDeleteReq()
{
    FileMetaData targetFile = selectedItem->getFileData();
    //TODO: verify file valid for delete
    DeleteConfirm deletePopup(targetFile.getFullPath());
    if (deletePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    qDebug("Starting delete procedure: %s",qPrintable(targetFile.getFullPath()));
    RemoteDataReply * theReply = dataLink->deleteFile(targetFile.getFullPath());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveDeleteReply(RequestState,QString*)),
                     this, SLOT(getDeleteReply(RequestState,QString*)));
    fileOperationPending = true;
}

void FileTreeModelReader::getDeleteReply(RequestState replyState, QString * oldFilePath)
{
    fileOperationPending = false;
    if (replyState == RequestState::GOOD)
    {
        FileTreeNode * toRemove = getFileNodeFromPath(*oldFilePath);
        if (toRemove != NULL)
        {
            enactFolderRefresh(toRemove->getParentNode());
            return;
        }

        toRemove = getFileNearestFromPath(*oldFilePath);
        enactFolderRefresh(toRemove);
    }
}

void FileTreeModelReader::sendMoveReq()
{
    //TODO
}

void FileTreeModelReader::getMoveReply(RequestState replyState, QString * oldFilePath, FileMetaData * revisedFileData)
{
    fileOperationPending = false;
    //TODO
}

void FileTreeModelReader::sendCopyReq()
{
    //TODO
}

void FileTreeModelReader::getCopyReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOperationPending = false;
    //TODO
}

void FileTreeModelReader::sendRenameReq()
{
    FileMetaData targetFile = selectedItem->getFileData();
    SingleLineDialog newNamePopup("Please type a new file name:", "newname");
    //TODO: NEED lots of verification here
    //First, that file can be renamed
    //Second, that new file name is valid
    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    qDebug("Starting rename procedure: %s to %s", qPrintable(targetFile.getFullPath()), qPrintable(newNamePopup.getInputText()));
    RemoteDataReply * theReply = dataLink->renameFile(targetFile.getFullPath(), newNamePopup.getInputText());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveRenameReply(RequestState,QString*,FileMetaData*)), this, SLOT(getRenameReply(RequestState,QString*,FileMetaData*)));
    fileOperationPending = true;
}

void FileTreeModelReader::getRenameReply(RequestState replyState, QString * oldFilePath, FileMetaData * newFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    FileTreeNode * toChange = getFileNodeFromPath(*oldFilePath);
    if (toChange == NULL)
    {
        return;
    }
    FileTreeNode * toCheck = toChange->getParentNode();
    enactFolderRefresh(toCheck);
    FileTreeNode * compareTo = getFileNodeFromPath(newFileData->getContainingPath());
    if (toCheck != compareTo)
    {
        enactFolderRefresh(compareTo);
    }
}

void FileTreeModelReader::sendCreateFolderReq()
{
    FileMetaData targetFile = selectedItem->getFileData();
    SingleLineDialog newFolderNamePopup("Please input a name for the new folder:", "newFolder1");
    //TODO: verification here
    //First, valid folder to create in
    //Second, that new name is valid
    if (newFolderNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    qDebug("Starting create folder procedure: %s at %s", qPrintable(newFolderNamePopup.getInputText()),
           qPrintable(targetFile.getFullPath()));
    RemoteDataReply * theReply = dataLink->mkRemoteDir(targetFile.getFullPath(), newFolderNamePopup.getInputText());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveMkdirReply(RequestState,FileMetaData*)),
                     this, SLOT(getMkdirReply(RequestState,FileMetaData*)));

    fileOperationPending = true;
}

void FileTreeModelReader::getMkdirReply(RequestState replyState, FileMetaData * newFolderData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    FileTreeNode * parentNode = getFileNearestFromPath(newFolderData->getContainingPath());
    enactFolderRefresh(parentNode);
}

void FileTreeModelReader::sendUploadReq()
{
    FileMetaData targetFile = selectedItem->getFileData();
    SingleLineDialog uploadNamePopup("Please input full path of file to upload:", "");
    //TODO: NEED lots of verification here
    //First, valid folder to upload to
    //Second, that uploaded file exists and is valid
    if (uploadNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    qDebug("Starting upload procedure: %s to %s", qPrintable(uploadNamePopup.getInputText()),
           qPrintable(targetFile.getFullPath()));
    RemoteDataReply * theReply = dataLink->uploadFile(targetFile.getFullPath(), uploadNamePopup.getInputText());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveUploadReply(RequestState,FileMetaData*)),
                     this, SLOT(getUploadReply(RequestState,FileMetaData*)));

    fileOperationPending = true;
}

void FileTreeModelReader::getUploadReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    FileTreeNode * parentNode = getFileNearestFromPath(newFileData->getContainingPath());
    enactFolderRefresh(parentNode);
}

void FileTreeModelReader::sendDownloadReq()
{
    //TODO
}

void FileTreeModelReader::getDownloadReply(RequestState replyState, QString * localDest)
{
    fileOperationPending = false;
    //TODO
}

void FileTreeModelReader::sendManualRefresh()
{
    enactFolderRefresh(selectedItem);
}

QString FileTreeModelReader::getFilePathForNode(QModelIndex dataIndex)
{
    FileTreeNode * nodeToCheck = getNodeFromModel(dataStore.itemFromIndex(dataIndex));
    if (nodeToCheck == NULL) return "";
    return nodeToCheck->getFileData().getFullPath();
}

FileTreeNode * FileTreeModelReader::getFileNodeFromPath(QString filePath)
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
FileTreeNode * FileTreeModelReader::getFileNearestFromPath(QString filePath)
{
    QStringList filePathParts = FileMetaData::getPathNameList(filePath);
    FileTreeNode * searchNode = rootFileNode;

    for (auto itr = filePathParts.cbegin(); itr != filePathParts.cend(); itr++)
    {
        FileTreeNode * nextNode = searchNode->getChildNodeWithName(*itr);
        if (nextNode == NULL)
        {
            return searchNode;
        }
        searchNode = nextNode;
    }

    return searchNode;
}

FileTreeNode * FileTreeModelReader::getNodeFromModel(QStandardItem * toFind)
{
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

QStandardItem * FileTreeModelReader::getModelEntryFromNode(FileTreeNode * toFind)
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

void FileTreeModelReader::totalResetErrorProcedure()
{
    //TODO: Try to recover once by resetting all data on remote files
    ErrorPopup("Critical Remote file parseing error. Unable to Recover");
}

void FileTreeModelReader::translateFileDataToModel()
{
    FileTreeNode * currentFile = rootFileNode;
    QStandardItem * currentModelEntry = dataStore.invisibleRootItem();

    translateFileDataRecurseHelper(currentFile, currentModelEntry);
}

void FileTreeModelReader::translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry)
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

void FileTreeModelReader::enactFolderRefresh(FileTreeNode * folderToRemoteLS)
{
    QString fullFilePath = folderToRemoteLS->getFileData().getFullPath();

    qDebug("File Path Needs refresh: %s", qPrintable(fullFilePath));
    RemoteDataReply * theReply = dataLink->remoteLS(fullFilePath);
    if (theReply == NULL)
    {
        //TODO: consider a more fatal error here
        totalResetErrorProcedure();
    }
    else
    {
        QObject::connect(theReply, SIGNAL(haveLSReply(RequestState,QList<FileMetaData>*)), this, SLOT(getLSReply(RequestState,QList<FileMetaData>*)));
    }
}

bool FileTreeModelReader::fileInModel(FileTreeNode * toFind, QStandardItem * compareTo)
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

void FileTreeModelReader::changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource)
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

void FileTreeModelReader::newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource)
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

bool FileTreeModelReader::columnInUse(int i)
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

QString FileTreeModelReader::getRawColumnData(int i, FileMetaData * rawFileData)
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

FileTreeNode::FileTreeNode(FileMetaData contents, FileTreeNode * parent):QObject((QObject *)parent)
{
    fileData = new FileMetaData(contents);
    childList = new QList<FileTreeNode *>();

    if (parent == NULL)
    {
        rootNode = true;
    }
    else
    {
        rootNode = false;
        parent->childList->append(this);
    }
}

FileTreeNode::FileTreeNode(FileTreeNode * parent):QObject((QObject *)parent)
{
    childList = new QList<FileTreeNode *>();

    if (parent == NULL)
    {
        rootNode = true;

        //Create default root folder
        fileData = new FileMetaData();
        fileData->setFullFilePath("/");
        fileData->setType(FileType::DIR);
    }
    else
    {
        rootNode = false;
        parent->childList->append(this);

        //Create loading placeholder
        QString fullPath = parent->fileData->getFullPath();
        fullPath.append("/Loading");
        fileData = new FileMetaData();
        fileData->setFullFilePath(fullPath);
        fileData->setType(FileType::UNLOADED);
    }
}

FileTreeNode::~FileTreeNode()
{
    if (this->parent() != NULL)
    {
        FileTreeNode * parentNode = (FileTreeNode *)this->parent();
        if (parentNode->childList->contains(this))
        {
            parentNode->childList->removeAll(this);
        }
    }
    if (this->fileData != NULL)
    {
        delete this->fileData;
    }
    if (this->childList != NULL)
    {
        while (this->childList->size() > 0)
        {
            FileTreeNode * toDelete = this->childList->takeLast();
            delete toDelete;
        }
        delete this->childList;
    }
}

QList<FileTreeNode *>::iterator FileTreeNode::fileListStart()
{
    return childList->begin();
}

QList<FileTreeNode *>::iterator FileTreeNode::fileListEnd()
{
    return childList->end();
}

bool FileTreeNode::isRootNode()
{
    return rootNode;
}

void FileTreeNode::setFileData(FileMetaData newData)
{
    if (fileData != NULL)
    {
        delete fileData;
    }
    fileData = new FileMetaData(newData);
}

FileTreeNode * FileTreeNode::getParentNode()
{
    FileTreeNode * ret = (FileTreeNode *)this->parent();
    return ret;
}

FileTreeNode * FileTreeNode::getChildNodeWithName(QString filename, bool unrestricted)
{
    for (auto itr = this->childList->begin(); itr != this->childList->end(); itr++)
    {
        FileMetaData toSearch = (*itr)->getFileData();
        if ((unrestricted) || (toSearch.getFileType() == FileType::DIR) || (toSearch.getFileType() == FileType::FILE))
        {
            if (toSearch.getFileName() == filename)
            {
                return (*itr);
            }
        }
    }
    return NULL;
}

void FileTreeNode::clearAllChildren()
{
    while (childList->size() > 0)
    {
        FileTreeNode *toDestroy = childList->takeLast();
        toDestroy->deleteLater();
    }
}

FileMetaData FileTreeNode::getFileData()
{
    return *fileData;
}

bool FileTreeNode::childIsUnloaded()
{
    for (auto itr = childList->cbegin(); itr != childList->cend(); itr++)
    {
        if ((*itr)->getFileData().getFileType() == FileType::UNLOADED)
        {
            return true;
        }
    }
    return false;
}

void FileTreeNode::setMark(bool newSetting)
{
    marked = newSetting;
}

bool FileTreeNode::isMarked()
{
    return marked;
}
