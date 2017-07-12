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

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"

#include "utilWindows/errorpopup.h"
#include "utilWindows/deleteconfirm.h"
#include "utilWindows/singlelinedialog.h"
#include "utilWindows/quickinfopopup.h"

FileOperator::FileOperator(RemoteDataInterface * newDataLink, QObject *parent) : QObject(parent)
{
    dataLink = newDataLink;
}

void FileOperator::linkToFileTree(RemoteFileTree * newTreeLink)
{
    newTreeLink->setModel(&dataStore);
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

void FileOperator::enactFolderRefresh(FileMetaData folderToRemoteLS)
{
    QString fullFilePath = folderToRemoteLS.getFullPath();

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
    return fileOperationPending;
}

void FileOperator::getLSReply(RequestState cmdReply, QList<FileMetaData> * fileDataList)
{
    if (cmdReply != RequestState::GOOD)
    {
        totalResetErrorProcedure();
        return;
    }
    myFileTree->updateFileInfo(fileDataList);
}

void FileOperator::sendDeleteReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
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
    QObject::connect(theReply, SIGNAL(haveDeleteReply(RequestState)),
                     this, SLOT(getDeleteReply(RequestState)));
    fileOperationPending = true;
}

void FileOperator::getDeleteReply(RequestState replyState)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNodeToParent(getStringFromInitParams("toDelete"));
}

void FileOperator::sendMoveReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
    SingleLineDialog newNamePopup("Please type a file name to move to:", "newname");
    //TODO: NEED lots of verification here
    //First, that file can be renamed
    //Second, that new file name is valid
    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    dataLink->setCurrentRemoteWorkingDirectory(targetFile.getContainingPath());

    qDebug("Starting move procedure: %s to %s", qPrintable(targetFile.getFullPath()), qPrintable(newNamePopup.getInputText()));
    RemoteDataReply * theReply = dataLink->moveFile(targetFile.getFullPath(), newNamePopup.getInputText());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveMoveReply(RequestState,FileMetaData*)), this, SLOT(getMoveReply(RequestState,FileMetaData*)));
    fileOperationPending = true;
}

void FileOperator::getMoveReply(RequestState replyState, FileMetaData * revisedFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNodeToParent(getStringFromInitParams("from"));
    myFileTree->lsClosestNode(revisedFileData->getFullPath());
}

void FileOperator::sendCopyReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
    SingleLineDialog newNamePopup("Please type a file name to copy to:", "newname");
    //TODO: NEED lots of verification here
    //First, that file can be renamed
    //Second, that new file name is valid
    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    dataLink->setCurrentRemoteWorkingDirectory(targetFile.getContainingPath());

    qDebug("Starting copy procedure: %s to %s", qPrintable(targetFile.getFullPath()), qPrintable(newNamePopup.getInputText()));
    RemoteDataReply * theReply = dataLink->copyFile(targetFile.getFullPath(), newNamePopup.getInputText());
    if (theReply == NULL)
    {
        //TODO, should have more meaningful error here
        return;
    }
    QObject::connect(theReply, SIGNAL(haveCopyReply(RequestState,FileMetaData*)), this, SLOT(getCopyReply(RequestState,FileMetaData*)));
    fileOperationPending = true;
}

void FileOperator::getCopyReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNode(newFileData->getFullPath());
}

void FileOperator::sendRenameReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
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
    QObject::connect(theReply, SIGNAL(haveRenameReply(RequestState,FileMetaData*)), this, SLOT(getRenameReply(RequestState,FileMetaData*)));
    fileOperationPending = true;
}

void FileOperator::getRenameReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNodeToParent(getStringFromInitParams("fullName"));
    myFileTree->lsClosestNodeToParent(newFileData->getFullPath());
}

void FileOperator::sendCreateFolderReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
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

void FileOperator::getMkdirReply(RequestState replyState, FileMetaData * newFolderData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNode(newFolderData->getContainingPath());
}

void FileOperator::sendUploadReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
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
        QuickInfoPopup downloadPopup("File Upload has failed");
        downloadPopup.exec();
        return;
    }
    QObject::connect(theReply, SIGNAL(haveUploadReply(RequestState,FileMetaData*)),
                     this, SLOT(getUploadReply(RequestState,FileMetaData*)));

    fileOperationPending = true;
}

void FileOperator::getUploadReply(RequestState replyState, FileMetaData * newFileData)
{
    fileOperationPending = false;
    if (replyState != RequestState::GOOD)
    {
        return;
    }

    myFileTree->lsClosestNode(newFileData->getFullPath());
}

void FileOperator::sendDownloadReq()
{
    FileMetaData targetFile = myFileTree->getCurrentSelectedFile();
    SingleLineDialog downloadNamePopup("Please input full path download destination:", "");
    //TODO: NEED lots of verification here
    //First, valid folder to upload to
    //Second, that download destination is valid
    if (downloadNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    qDebug("Starting download procedure: %s to %s", qPrintable(targetFile.getFullPath()),
           qPrintable(downloadNamePopup.getInputText()));
    RemoteDataReply * theReply = dataLink->downloadFile(downloadNamePopup.getInputText(), targetFile.getFullPath());
    if (theReply == NULL)
    {
        QuickInfoPopup downloadPopup("Unable to download file. Check that destination file does not already exist");
        downloadPopup.exec();
        return;
    }
    QObject::connect(theReply, SIGNAL(haveDownloadReply(RequestState)),
                     this, SLOT(getDownloadReply(RequestState)));

    fileOperationPending = true;
}

void FileOperator::getDownloadReply(RequestState replyState)
{
    fileOperationPending = false;
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

void FileOperator::sendCompressReq()
{
    qDebug("Folder compress specified");
    QMultiMap<QString, QString> oneInput;
    oneInput.insert("compression_type","tgz");
    FileMetaData fileData = myFileTree->getCurrentSelectedFile();
    if (fileData.getFileType() != FileType::DIR)
    {
        //TODO: give reasonable error
        return;
    }
    RemoteDataReply * compressTask = dataLink->runRemoteJob("compress",oneInput,fileData.getFullPath());
    if (compressTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(compressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(getCompressReply(RequestState,QJsonDocument*)));
}

void FileOperator::getCompressReply(RequestState finalState, QJsonDocument *)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

void FileOperator::sendDecompressReq()
{
    qDebug("Folder de-compress specified");
    QMultiMap<QString, QString> oneInput;
    FileMetaData fileData = myFileTree->getCurrentSelectedFile();
    if (fileData.getFileType() == FileType::DIR)
    {
        //TODO: give reasonable error
        return;
    }
    oneInput.insert("inputFile",fileData.getFullPath());

    RemoteDataReply * decompressTask = dataLink->runRemoteJob("extract",oneInput,"");
    if (decompressTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(decompressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(getDecompressReply(RequestState,QJsonDocument*)));
}

void FileOperator::getDecompressReply(RequestState finalState, QJsonDocument *)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

void FileOperator::sendManualRefresh()
{
    enactFolderRefresh(myFileTree->getCurrentSelectedFile());
}
