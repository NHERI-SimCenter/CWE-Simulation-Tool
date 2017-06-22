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

class RemoteFileTree;
class FileMetaData;
class RemoteDataInterface;

enum class RequestState;

class FileOperator : public QObject
{
    Q_OBJECT

public:
    FileOperator(RemoteDataInterface * newDataLink, RemoteFileTree * parent);

    void totalResetErrorProcedure();
    void enactFolderRefresh(FileMetaData folderToRemoteLS);
    bool operationIsPending();

private slots:
    void getLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList);

    void sendDeleteReq();
    void getDeleteReply(RequestState replyState);
    void sendMoveReq();
    void getMoveReply(RequestState replyState, FileMetaData * revisedFileData);
    void sendCopyReq();
    void getCopyReply(RequestState replyState, FileMetaData * newFileData);
    void sendRenameReq();
    void getRenameReply(RequestState replyState, FileMetaData * newFileData);

    void sendCreateFolderReq();
    void getMkdirReply(RequestState replyState, FileMetaData * newFolderData);

    void sendUploadReq();
    void getUploadReply(RequestState replyState, FileMetaData * newFileData);
    void sendDownloadReq();
    void getDownloadReply(RequestState replyState);

    void sendCompressReq();
    void getCompressReply(RequestState finalState, QJsonDocument * rawData);
    void sendDecompressReq();
    void getDecompressReply(RequestState finalState, QJsonDocument * rawData);

    void sendManualRefresh();

private:
    QString getStringFromInitParams(QString stringKey);

    RemoteFileTree * myFileTree;

    RemoteDataInterface * dataLink;
    bool fileOperationPending = false;
};

#endif // FILEOPERATOR_H
