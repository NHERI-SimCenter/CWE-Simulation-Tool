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

#include "agavetaskreply.h"

#include "agavetaskguide.h"
#include "agavehandler.h"
#include "../programWindows/errorpopup.h"

AgaveTaskReply::AgaveTaskReply(AgaveTaskGuide * theGuide, QNetworkReply * newReply, QObject *parent) : RemoteDataReply(parent)
{
    myGuide = theGuide;
    if (myGuide == NULL)
    {
        ErrorPopup("Task Reply has no task guide.");
        return;
    }
    myReplyObject = newReply;
    if ((myReplyObject == NULL) && (myGuide->getRequestType() != AgaveRequestType::AGAVE_NONE))
    {
        ErrorPopup("Task Reply has no network reply.");
        return;
    }

    if (myReplyObject != NULL)
    {
        QObject::connect(myReplyObject, SIGNAL(finished()), this, SLOT(rawTaskComplete()));
    }
}

AgaveTaskReply::~AgaveTaskReply()
{
    if (myReplyObject != NULL)
    {
        myReplyObject->deleteLater();
    }
}

void AgaveTaskReply::delayedPassThruReply(RequestState replyState, QString * param1)
{
    if (myGuide->getRequestType() != AgaveRequestType::AGAVE_NONE)
    {
        ErrorPopup("Passthru reply invoked on invalid task");
        return;
    }

    haveReplyStore = true;
    pendingReply = replyState;
    pendingParam = *param1;

    QTimer * quickTimer = new QTimer((QObject*)this);
    QObject::connect(quickTimer, SIGNAL(timeout()), this, SLOT(rawTaskComplete()));
    quickTimer->start(2);
}

void AgaveTaskReply::invokePassThruReply(RequestState replyState, QString * param1)
{
    this->deleteLater();
    if (myGuide->getRequestType() != AgaveRequestType::AGAVE_NONE)
    {
        ErrorPopup("Passthru reply invoked on invalid task");
        return;
    }
    if (myGuide->getTaskID() == "changeDir")
    {
        emit haveCurrentRemoteDir(replyState, param1);
        return;
    }
    if (myGuide->getTaskID() == "fullAuth")
    {
        emit haveAuthReply(replyState);
        return;
    }
    ErrorPopup("Passthru reply not implemented");
    return;
}

AgaveTaskGuide * AgaveTaskReply::getTaskGuide()
{
    return myGuide;
}

void AgaveTaskReply::setDataStore(QString newSetting)
{
    dataStore = newSetting;
}

void AgaveTaskReply::processNoContactReply(QString errorText)
{
    processBadReply(RequestState::NO_CONNECT, errorText);
}

void AgaveTaskReply::processFailureReply(QString errorText)
{
    processBadReply(RequestState::FAIL, errorText);
}

void AgaveTaskReply::processBadReply(RequestState replyState, QString errorText)
{
    qDebug("%s", qPrintable(errorText));

    if (myGuide->getTaskID() == "changeDir")
    {
        ErrorPopup("Change Dir failed.");
    }
    else if (myGuide->getTaskID() == "authRefresh")
    {
        ErrorPopup("Auth refresh not implemented");
    }
    else if (myGuide->getTaskID() == "dirListing")
    {
        emit haveLSReply(replyState, NULL);
    }
    else if (myGuide->getTaskID() == "fileUpload")
    {
        emit haveUploadReply(replyState, NULL);
    }
    else if (myGuide->getTaskID() == "fileDelete")
    {
        emit haveDeleteReply(replyState, NULL);
    }
    else if (myGuide->getTaskID() == "newFolder")
    {
        emit haveMkdirReply(replyState, NULL);
    }
    else if (myGuide->getTaskID() == "renameFile")
    {
        emit haveRenameReply(replyState, NULL, NULL);
    }
    else
    {
        emit haveJobReply(replyState, NULL);
    }
}

void AgaveTaskReply::rawTaskComplete()
{
    if (myGuide->getRequestType() == AgaveRequestType::AGAVE_NONE)
    {
        if (haveReplyStore == false)
        {
            invokePassThruReply(RequestState::NO_CONNECT);
        }
        else
        {
            invokePassThruReply(pendingReply, &pendingParam);
        }
        return;
    }

    this->deleteLater();

    QNetworkReply * testReply = (QNetworkReply*)QObject::sender();
    if (testReply != myReplyObject)
    {
        ErrorPopup("Network reply does not match agave reply");
        return;
    }

    //If this task is an INTERNAL task, then the result is redirected to the manager
    if (myGuide->isInternal())
    {
        emit haveInternalTaskReply(this, myReplyObject);
        return;
    }

    if (myGuide->getTaskID() == "authRevoke")
    {
        //As far as I can tell, token revokes give no reply
        qDebug("Token revoke complete.");
        emit connectionsClosed(RequestState::GOOD);
        return;
    }

    const QByteArray replyText = myReplyObject->readAll();

    QJsonParseError parseError;
    QJsonDocument parseHandler = QJsonDocument::fromJson(replyText, &parseError);

    if (parseHandler.isNull())
    {
        if ((int)myReplyObject->error() != 0)
        {
            qDebug("%d", (int)myReplyObject->error());
            processFailureReply(myReplyObject->errorString());
        }
        else
        {
            processNoContactReply("JSON parse failed");
        }
        return;
    }

    qDebug("%s", qPrintable(parseHandler.toJson()));

    RequestState prelimResult = standardSuccessFailCheck(myGuide, &parseHandler);

    if (prelimResult == RequestState::NO_CONNECT)
    {
        processNoContactReply("Missing Status String");
    }
    else if (prelimResult == RequestState::FAIL)
    {
        processFailureReply("Request rejected by remote system");
    }

    if (myGuide->getTaskID() == "authRefresh")
    {
        ErrorPopup("Auth refresh not implemented yet");
    }
    else if (myGuide->getTaskID() == "dirListing")
    {
        QJsonValue expectedArray = retriveMainAgaveJSON(&parseHandler,"result");
        if (!expectedArray.isArray())
        {
            processFailureReply("Parse gives no array for file list.");
            return;
        }
        QJsonArray fileArray = expectedArray.toArray();
        QList<FileMetaData> fileList;
        for (auto itr = fileArray.constBegin(); itr != fileArray.constEnd(); itr++)
        {
            FileMetaData aFile = parseJSONfileMetaData((*itr).toObject());
            if (aFile.getFileType() == FileType::INVALID)
            {
                processFailureReply("Parse gives invalid array for file list.");
                return;
            }
            fileList.append(aFile);
        }
        emit haveLSReply(RequestState::GOOD, &fileList);
    }
    else if (myGuide->getTaskID() == "fileUpload")
    {
        QJsonValue expectedObject = retriveMainAgaveJSON(&parseHandler,"result");
        FileMetaData aFile = parseJSONfileMetaData(expectedObject.toObject());
        if (aFile.getFileType() == FileType::INVALID)
        {
            processFailureReply("Invalid file data");
            return;
        }
        emit haveUploadReply(RequestState::GOOD, &aFile);
    }
    else if (myGuide->getTaskID() == "fileDelete")
    {
        emit haveDeleteReply(RequestState::GOOD, &dataStore);
    }
    else if (myGuide->getTaskID() == "newFolder")
    {
        QJsonValue expectedObject = retriveMainAgaveJSON(&parseHandler,"result");
        FileMetaData aFile = parseJSONfileMetaData(expectedObject.toObject());
        if (aFile.getFileType() == FileType::INVALID)
        {
            processFailureReply("Invalid file data");
            return;
        }
        emit haveMkdirReply(RequestState::GOOD, &aFile);
    }
    else if (myGuide->getTaskID() == "renameFile")
    {
        QJsonValue expectedObject = retriveMainAgaveJSON(&parseHandler,"result");
        FileMetaData aFile = parseJSONfileMetaData(expectedObject.toObject());
        if (aFile.getFileType() == FileType::INVALID)
        {
            processFailureReply("Invalid file data");
            return;
        }
        emit haveRenameReply(RequestState::GOOD, &dataStore, &aFile);
    }
    else
    {
        emit haveJobReply(RequestState::GOOD, &parseHandler);
    }
}

RequestState AgaveTaskReply::standardSuccessFailCheck(AgaveTaskGuide * taskGuide, QJsonDocument * parsedDoc)
{
    //In Agave TOKEN uses a different output form
    if (taskGuide->isTokenFormat())
    {
        if (parsedDoc->object().contains("error"))
        {
            return RequestState::FAIL;
        }
    }
    else
    {
        QString statusString = retriveMainAgaveJSON(parsedDoc,"status").toString();

        if (statusString == "error")
        {
            return RequestState::FAIL;
        }
        else if (statusString != "success")
        {
            return RequestState::NO_CONNECT;
        }
    }
    return RequestState::GOOD;
}

FileMetaData AgaveTaskReply::parseJSONfileMetaData(QJsonObject fileNameValuePairs)
{
    FileMetaData ret;
    if (!(fileNameValuePairs.contains("format") || fileNameValuePairs.contains("nativeFormat"))
            || !fileNameValuePairs.contains("name")
            || !fileNameValuePairs.contains("path")
            || !fileNameValuePairs.value("path").isString())
    {
        return ret;
    }

    if (fileNameValuePairs.value("name").toString() == ".")
    {
        QString tmp = fileNameValuePairs.value("path").toString();
        tmp.append("/.");
        ret.setFullFilePath(tmp);
    }
    else
    {
        ret.setFullFilePath(fileNameValuePairs.value("path").toString());
    }

    QString typeString = fileNameValuePairs.value("type").toString();
    if (typeString.isEmpty())
    {
        typeString = fileNameValuePairs.value("nativeFormat").toString();
    }
    if (typeString == "dir")
    {
        ret.setType(FileType::DIR);
    }
    else if (typeString == "file")
    {
        ret.setType(FileType::FILE);
    }
    else if (typeString == "raw")
    {
        ret.setType(FileType::FILE);
    }
    //TODO: consider more validity checks here
    int fileLength = fileNameValuePairs.value("length").toInt();
    ret.setSize(fileLength);

    return ret;
}

QJsonValue AgaveTaskReply::retriveMainAgaveJSON(QJsonDocument * parsedDoc, const char * oneKey)
{
    QList<QString> smallList = { oneKey };
    return retriveMainAgaveJSON(parsedDoc, smallList);
}

QJsonValue AgaveTaskReply::retriveMainAgaveJSON(QJsonDocument * parsedDoc, QString oneKey)
{
    QList<QString> smallList = { oneKey };
    return retriveMainAgaveJSON(parsedDoc, smallList);
}

QJsonValue AgaveTaskReply::retriveMainAgaveJSON(QJsonDocument * parsedDoc, QList<QString> keyList)
{
    QJsonValue nullVal;
    if (keyList.size() < 1) return nullVal;
    if (parsedDoc->isNull()) return nullVal;
    if (parsedDoc->isEmpty()) return nullVal;
    if (!parsedDoc->isObject()) return nullVal;

    QJsonValue resultVal = recursiveJSONdig(QJsonValue(parsedDoc->object()) , &keyList, 0);
    if (resultVal.isUndefined()) return nullVal;
    if (resultVal.isNull()) return nullVal;
    return resultVal;
}

QJsonValue AgaveTaskReply::recursiveJSONdig(QJsonValue currVal, QList<QString> * keyList, int i)
{
    QJsonValue nullValue;
    if (i >= keyList->size())
    {
        //Should never happen
        ErrorPopup("Invalid JSON parser state");
        return nullValue;
    }

    QString keyToFind = keyList->at(i);

    //Get next obj
    if (!currVal.isObject()) return nullValue;
    if (!currVal.toObject().contains(keyToFind)) return nullValue;
    QJsonValue targetedValue = currVal.toObject().value(keyToFind);
    if (targetedValue.isUndefined()) return nullValue;

    if (i == keyList->size() - 1)
    {
        return targetedValue;
    }

    return recursiveJSONdig(targetedValue,keyList,i+1);
}
