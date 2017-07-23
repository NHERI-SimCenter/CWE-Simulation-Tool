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

#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <QObject>
#include <QList>
#include <QStandardItemModel>
#include <QStandardItem>

class RemoteFileTree;
class FileMetaData;
class RemoteDataInterface;
class FileTreeNode;
class EasyBoolLock;

enum class RequestState;

class FileOperator : public QObject
{
    Q_OBJECT

public:
    FileOperator(RemoteDataInterface * newDataLink, QObject * parent);
    void linkToFileTree(RemoteFileTree * newTreeLink);

    void resetFileData();

    void totalResetErrorProcedure();
    bool operationIsPending();

    FileTreeNode * getNodeFromIndex(QModelIndex fileIndex);

    void translateFileDataToModel();

    void lsClosestNode(QString fullPath);
    void lsClosestNodeToParent(QString fullPath);

    void enactFolderRefresh(FileTreeNode * selectedNode);

    void sendDeleteReq(FileTreeNode * selectedNode);
    void sendMoveReq(FileTreeNode * moveFrom, QString newName);
    void sendCopyReq(FileTreeNode * copyFrom, QString newName);
    void sendRenameReq(FileTreeNode * selectedNode, QString newName);

    void sendCreateFolderReq(FileTreeNode * selectedNode, QString newName);

    void sendUploadReq(FileTreeNode * uploadTarget, QString localFile);
    void sendDownloadReq(FileTreeNode * targetFile, QString localDest);

    void sendCompressReq(FileTreeNode * selectedFolder);
    void sendDecompressReq(FileTreeNode * selectedFolder);

signals:
    void opPendingChange(bool opPending);

private slots:
    void opLockChanged(bool newVal);

    void getLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList);

    void getDeleteReply(RequestState replyState);
    void getMoveReply(RequestState replyState, FileMetaData * revisedFileData);
    void getCopyReply(RequestState replyState, FileMetaData * newFileData);
    void getRenameReply(RequestState replyState, FileMetaData * newFileData);

    void getMkdirReply(RequestState replyState, FileMetaData * newFolderData);

    void getUploadReply(RequestState replyState, FileMetaData * newFileData);
    void getDownloadReply(RequestState replyState);

    void getCompressReply(RequestState finalState, QJsonDocument * rawData);
    void getDecompressReply(RequestState finalState, QJsonDocument * rawData);

private:
    bool columnInUse(int i);
    QString getRawColumnData(int i, FileMetaData * rawFileData);
    QString getStringFromInitParams(QString stringKey);

    //Note: if not found, will return NULL and call translateFileDataToModel(), to resync
    //If input is NULL, return NULL, but don't resync
    FileTreeNode * getNodeFromModel(QStandardItem * toFind);
    QStandardItem * getModelEntryFromNode(FileTreeNode * toFind);

    void translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry);

    bool fileInModel(FileTreeNode * toFind, QStandardItem * compareTo);
    void changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource);
    void newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource);

    RemoteDataInterface * dataLink;
    FileTreeNode * rootFileNode = NULL;
    QStandardItemModel dataStore;

    EasyBoolLock * fileOpPending;

    const int tableNumCols = 7;
    const QStringList shownHeaderLabelList = {"File Name","Type","Size","Last Changed",
                                   "Format","mimeType","Permissions"};
    const QStringList hiddenHeaderLabelList = {"name","type","length","lastModified",
                                   "format","mimeType","permissions"};
};

#endif // FILEOPERATOR_H
