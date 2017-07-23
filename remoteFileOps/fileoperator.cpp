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

#include "fileoperator.h"

#include "remotefiletree.h"
#include "filetreenode.h"
#include "easyboollock.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"

#include "utilWindows/errorpopup.h"
#include "utilWindows/deleteconfirm.h"
#include "utilWindows/singlelinedialog.h"
#include "utilWindows/quickinfopopup.h"

FileOperator::FileOperator(RemoteDataInterface * newDataLink, QObject *parent) : QObject(parent)
{
    dataLink = newDataLink;

    //Note: will be deconstructed with parent
    fileOpPending = new EasyBoolLock(this);
    QObject::connect(fileOpPending, SIGNAL(lockStateChanged(bool)),
                     this, SLOT(opLockChanged(bool)));
}

void FileOperator::linkToFileTree(RemoteFileTree * newTreeLink)
{
    newTreeLink->setModel(&dataStore);
}

void FileOperator::resetFileData()
{
    dataStore.clear();
    dataStore.setColumnCount(tableNumCols);
    dataStore.setHorizontalHeaderLabels(shownHeaderLabelList);

    if (rootFileNode != NULL)
    {
        rootFileNode->deleteLater();
    }
    rootFileNode = new FileTreeNode();
    translateFileDataToModel();

    enactFolderRefresh(rootFileNode);
}

void FileOperator::totalResetErrorProcedure()
{
    //TODO: Try to recover once by resetting all data on remote files
    ErrorPopup("Critical Remote file parseing error. Unable to Recover");
}

QString FileOperator::getStringFromInitParams(QString stringKey)
{
    QString ret;
    RemoteDataReply * theReply = qobject_cast<RemoteDataReply *> (QObject::sender());
    if (theReply == NULL)
    {
        ErrorPopup("Unable to get sender object of reply signal");
        return ret;
    }
    ret = theReply->getTaskParamList()->value(stringKey);
    if (ret.isEmpty())
    {
        ErrorPopup("Missing init param");
        return ret;
    }
    return ret;
}

void FileOperator::enactFolderRefresh(FileTreeNode * selectedNode)
{
    QString fullFilePath = selectedNode->getFileData().getFullPath();

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

bool FileOperator::operationIsPending()
{
    return fileOpPending->lockClosed();
}

void FileOperator::getLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList)
{
    if (cmdReply != RequestState::GOOD)
    {
        totalResetErrorProcedure();
        return;
    }
    rootFileNode->updateFileFolder(*fileDataList);
    translateFileDataToModel();
}

void FileOperator::opLockChanged(bool newVal)
{
    emit opPendingChange(newVal);
}

void FileOperator::sendDeleteReq(FileTreeNode * selectedNode)
{
    if (!fileOpPending->checkAndClaim()) return;

    QString targetFile = selectedNode->getFileData().getFullPath();
    qDebug("Starting delete procedure: %s",qPrintable(targetFile));
    RemoteDataReply * theReply = dataLink->deleteFile(targetFile);
    if (theReply == NULL)
    {
        fileOpPending->release();
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveDeleteReply(RequestState)),
                     this, SLOT(getDeleteReply(RequestState)));
}

void FileOperator::getDeleteReply(RequestState replyState)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNodeToParent(getStringFromInitParams("toDelete"));
}

void FileOperator::sendMoveReq(FileTreeNode * moveFrom, QString newName)
{
    if (!fileOpPending->checkAndClaim()) return;
    dataLink->setCurrentRemoteWorkingDirectory(moveFrom->getFileData().getContainingPath());

    qDebug("Starting move procedure: %s to %s",
           qPrintable(moveFrom->getFileData().getFullPath()),
           qPrintable(newName));
    RemoteDataReply * theReply = dataLink->moveFile(moveFrom->getFileData().getFullPath(), newName);
    if (theReply == NULL)
    {
        fileOpPending->release();
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveMoveReply(RequestState,FileMetaData*)), this, SLOT(getMoveReply(RequestState,FileMetaData*)));
}

void FileOperator::getMoveReply(RequestState replyState, FileMetaData * revisedFileData)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNodeToParent(getStringFromInitParams("from"));
    lsClosestNode(revisedFileData->getFullPath());
}

void FileOperator::sendCopyReq(FileTreeNode * copyFrom, QString newName)
{
    if (!fileOpPending->checkAndClaim()) return;
    dataLink->setCurrentRemoteWorkingDirectory(copyFrom->getFileData().getContainingPath());

    qDebug("Starting copy procedure: %s to %s",
           qPrintable(copyFrom->getFileData().getFullPath()),
           qPrintable(newName));
    RemoteDataReply * theReply = dataLink->copyFile(copyFrom->getFileData().getFullPath(), newName);
    if (theReply == NULL)
    {
        fileOpPending->release();
        //TODO: Better error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveCopyReply(RequestState,FileMetaData*)), this, SLOT(getCopyReply(RequestState,FileMetaData*)));
}

void FileOperator::getCopyReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNode(newFileData->getFullPath());
}

//DOLINE

void FileOperator::sendRenameReq(FileTreeNode * selectedNode, QString newName)
{
    if (!fileOpPending->checkAndClaim()) return;

    qDebug("Starting rename procedure: %s to %s",
           qPrintable(selectedNode->getFileData().getFullPath()),
           qPrintable(newName));
    RemoteDataReply * theReply = dataLink->renameFile(selectedNode->getFileData().getFullPath(), newName);
    if (theReply == NULL)
    {
        fileOpPending->release();
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveRenameReply(RequestState,FileMetaData*)), this, SLOT(getRenameReply(RequestState,FileMetaData*)));
}

void FileOperator::getRenameReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNodeToParent(getStringFromInitParams("fullName"));
    lsClosestNodeToParent(newFileData->getFullPath());
}

void FileOperator::sendCreateFolderReq(FileTreeNode * selectedNode, QString newName)
{
    if (!fileOpPending->checkAndClaim()) return;

    qDebug("Starting create folder procedure: %s at %s",
           qPrintable(selectedNode->getFileData().getFullPath()),
           qPrintable(newName));
    RemoteDataReply * theReply = dataLink->mkRemoteDir(selectedNode->getFileData().getFullPath(), newName);
    if (theReply == NULL)
    {
        fileOpPending->release();
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveMkdirReply(RequestState,FileMetaData*)),
                     this, SLOT(getMkdirReply(RequestState,FileMetaData*)));
}

void FileOperator::getMkdirReply(RequestState replyState, FileMetaData * newFolderData)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNode(newFolderData->getContainingPath());
}

void FileOperator::sendUploadReq(FileTreeNode * uploadTarget, QString localFile)
{
    if (!fileOpPending->checkAndClaim()) return;
    qDebug("Starting upload procedure: %s to %s", qPrintable(localFile),
           qPrintable(uploadTarget->getFileData().getFullPath()));
    RemoteDataReply * theReply = dataLink->uploadFile(uploadTarget->getFileData().getFullPath(), localFile);
    if (theReply == NULL)
    {
        fileOpPending->release();
        return;
    }
    QObject::connect(theReply, SIGNAL(haveUploadReply(RequestState,FileMetaData*)),
                     this, SLOT(getUploadReply(RequestState,FileMetaData*)));
}

void FileOperator::getUploadReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    lsClosestNode(newFileData->getFullPath());
}

void FileOperator::sendDownloadReq(FileTreeNode * targetFile, QString localDest)
{
    if (!fileOpPending->checkAndClaim()) return;
    qDebug("Starting download procedure: %s to %s", qPrintable(targetFile->getFileData().getFullPath()),
           qPrintable(localDest));
    RemoteDataReply * theReply = dataLink->downloadFile(localDest, targetFile->getFileData().getFullPath());
    if (theReply == NULL)
    {
        fileOpPending->release();
        return;
    }
    QObject::connect(theReply, SIGNAL(haveDownloadReply(RequestState)),
                     this, SLOT(getDownloadReply(RequestState)));
}

void FileOperator::getDownloadReply(RequestState replyState)
{
    fileOpPending->release();
    if (replyState != RequestState::GOOD)
    {
        QuickInfoPopup downloadPopup("Error: Unable to download requested file.");
        downloadPopup.exec();
    }
    else
    {
        QuickInfoPopup downloadPopup(QString("Download complete to: %1").arg(getStringFromInitParams("localDest")));
        downloadPopup.exec();
    }
}

void FileOperator::sendCompressReq(FileTreeNode * selectedFolder)
{
    if (!fileOpPending->checkAndClaim()) return;
    qDebug("Folder compress specified");
    QMultiMap<QString, QString> oneInput;
    oneInput.insert("compression_type","tgz");
    FileMetaData fileData = selectedFolder->getFileData();
    if (fileData.getFileType() != FileType::DIR)
    {
        fileOpPending->release();
        //TODO: give reasonable error
        return;
    }
    RemoteDataReply * compressTask = dataLink->runRemoteJob("compress",oneInput,fileData.getFullPath());
    if (compressTask == NULL)
    {
        fileOpPending->release();
        //TODO: give reasonable error
        return;
    }
    QObject::connect(compressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(getCompressReply(RequestState,QJsonDocument*)));
}

void FileOperator::getCompressReply(RequestState finalState, QJsonDocument *)
{
    fileOpPending->release();
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

void FileOperator::sendDecompressReq(FileTreeNode * selectedFolder)
{
    if (!fileOpPending->checkAndClaim()) return;
    qDebug("Folder de-compress specified");
    QMultiMap<QString, QString> oneInput;
    FileMetaData fileData = selectedFolder->getFileData();
    if (fileData.getFileType() == FileType::DIR)
    {
        fileOpPending->release();
        //TODO: give reasonable error
        return;
    }
    oneInput.insert("inputFile",fileData.getFullPath());

    RemoteDataReply * decompressTask = dataLink->runRemoteJob("extract",oneInput,"");
    if (decompressTask == NULL)
    {
        fileOpPending->release();
        //TODO: give reasonable error
        return;
    }
    QObject::connect(decompressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(getDecompressReply(RequestState,QJsonDocument*)));
}

void FileOperator::getDecompressReply(RequestState finalState, QJsonDocument *)
{
    fileOpPending->release();
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

void FileOperator::lsClosestNode(QString fullPath)
{
    FileTreeNode * nodeToRefresh = rootFileNode->getClosestNodeWithName(fullPath);
    enactFolderRefresh(nodeToRefresh);
}

void FileOperator::lsClosestNodeToParent(QString fullPath)
{
    FileTreeNode * nodeToRefresh = rootFileNode->getNodeWithName(fullPath);
    if (nodeToRefresh != NULL)
    {
        if (!nodeToRefresh->isRootNode())
        {
            nodeToRefresh = nodeToRefresh->getParentNode();
        }
        enactFolderRefresh(nodeToRefresh);
        return;
    }

    nodeToRefresh = rootFileNode->getClosestNodeWithName(fullPath);
    enactFolderRefresh(nodeToRefresh);
}

FileTreeNode * FileOperator::getNodeFromModel(QStandardItem * toFind)
{
    if (toFind->parent() == NULL)
    {
        return rootFileNode;
    }

    int colNum = toFind->column();
    if (colNum != 0)
    {
        toFind = toFind->parent()->child(toFind->row(), 0);
    }
    QString realPath;
    while (toFind != NULL)
    {
        realPath = realPath.prepend(toFind->text());
        realPath = realPath.prepend("/");

        toFind = toFind->parent();
    }
    return rootFileNode->getNodeWithName(realPath);
}

QStandardItem * FileOperator::getModelEntryFromNode(FileTreeNode * toFind)
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

FileTreeNode * FileOperator::getNodeFromIndex(QModelIndex fileIndex)
{
    QStandardItem * theModelItem = dataStore.itemFromIndex(fileIndex);
    return getNodeFromModel(theModelItem);
}

void FileOperator::translateFileDataToModel()
{
    FileTreeNode * currentFile = rootFileNode;
    QStandardItem * currentModelEntry = dataStore.invisibleRootItem();

    translateFileDataRecurseHelper(currentFile, currentModelEntry);
}

void FileOperator::translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry)
{
    //TODO: I am guessing this could be more efficient
    QList<FileTreeNode *> * childList = currentFile->getChildList();
    for (auto itr = childList->begin(); itr != childList->end(); itr++)
    {
        (*itr)->marked = false;
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
            testFile->marked = true;
        }
    }

    for (auto itr = childList->begin(); itr != childList->end(); itr++)
    {
        if ((*itr)->marked == false)
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

bool FileOperator::fileInModel(FileTreeNode * toFind, QStandardItem * compareTo)
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

void FileOperator::changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource)
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

void FileOperator::newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource)
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

bool FileOperator::columnInUse(int i)
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

QString FileOperator::getRawColumnData(int i, FileMetaData * rawFileData)
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
