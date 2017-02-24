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

AgaveTaskReply::AgaveTaskReply(AgaveHandler * theManager, AgaveTaskGuide theGuide, QNetworkReply * newReply, QObject *parent) : QObject(parent)
{
    myManager = theManager;
    myGuide = theGuide;
    myReplyObject = newReply;
    if (myReplyObject == NULL)
    {
        validRequest = false;
        this->deleteLater();
        return;
    }
    myNetworkManager = myReplyObject->manager();

    QObject::connect(myReplyObject, SIGNAL(finished()), this, SLOT(rawTaskComplete()));
}

AgaveTaskReply::~AgaveTaskReply()
{
    if (myReplyObject != NULL)
    {
        myReplyObject->deleteLater();
    }
}

bool AgaveTaskReply::isValidRequest()
{
    return validRequest;
}

void AgaveTaskReply::rawTaskComplete()
{
    QNetworkReply * testReply = (QNetworkReply*)QObject::sender();
    if (testReply != myReplyObject)
    {
        ErrorPopup("Network reply does not match agave reply");
        return;
    }
    if ((int)myReplyObject->error() != 0)
    {
        qDebug("%s", myReplyObject->errorString().toStdString().c_str());
        qDebug("%d", (int)myReplyObject->error());
    }

    if (myGuide.getTaskID() == "AuthRevoke")
    {
        if (myManager->getState() == AgaveState::SHUTDOWN)
        {
            QMap<QString, QJsonValue> emptyDoc;
            processReqResult(RequestState::GOOD, emptyDoc);
        }
        else
        {
            ErrorPopup("Revoke detected outside of shutdown");
        }
        return;
    }

    if (!myGuide.verifyValidReplyState(myManager->getState()))
    {
        processReqResult();
        return;
    }

    const QByteArray replyText = myReplyObject->readAll();

    QJsonParseError parseError;
    QJsonDocument parseHandler = QJsonDocument::fromJson(replyText, &parseError);

    //qDebug(replyText);

    if (parseHandler.isNull())
    {
        qDebug("Parse failed");
        processReqResult();
        return;
    }

    QMap<QString, QList<QString>> relevantOutputSig;
    RequestState expectedState = RequestState::NO_CONNECT;

    //In Agave TOKEN uses a different output form
    if (myGuide.isTokenFormat())
    {
        if (parseHandler.object().contains("error"))
        {
            relevantOutputSig = myGuide.getOutputSignatureFail();
            expectedState = RequestState::FAIL;
        }
        else
        {
            relevantOutputSig = myGuide.getOutputSignatureGood();
            expectedState = RequestState::GOOD;
            for (auto itr = relevantOutputSig.cbegin(); itr != relevantOutputSig.cend(); ++itr)
            {
                QJsonValue valueCheck = retriveMainAgaveJSON(parseHandler, itr.value());
                if (valueCheck.isUndefined() || valueCheck.isNull())
                {
                    qDebug("In token format, val check failed");
                    processReqResult();
                    return;
                }
            }
        }
    }
    else
    {
        QString statusString = retriveMainAgaveJSON(parseHandler,"status").toString();

        if (statusString == "success")
        {
            relevantOutputSig = myGuide.getOutputSignatureGood();
            expectedState = RequestState::GOOD;
        }
        else if (statusString == "error")
        {
            relevantOutputSig = myGuide.getOutputSignatureFail();
            expectedState = RequestState::FAIL;
        }
        else
        {
            qDebug("Missing Status String");
            processReqResult();
            return;
        }
    }

    QMap<QString, QJsonValue> rawOutData;

    for (auto itr = relevantOutputSig.cbegin(); itr != relevantOutputSig.cend(); ++itr)
    {
        rawOutData.insert(itr.key(), retriveMainAgaveJSON(parseHandler, itr.value()));
    }

    processReqResult(expectedState, rawOutData);
}

void AgaveTaskReply::processReqResult()
{
    //This version assumes failure to connect
    QMap<QString,QJsonValue> emptyDoc;
    processReqResult(RequestState::NO_CONNECT, emptyDoc);
}

void AgaveTaskReply::processReqResult(RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    this->deleteLater();
    emit sendAgaveResultData(myGuide.getTaskID(),resultState,rawData);
}

QJsonValue AgaveTaskReply::retriveMainAgaveJSON(QJsonDocument parsedDoc, QString oneKey)
{
    QList<QString> smallList = { oneKey };
    return retriveMainAgaveJSON(parsedDoc, smallList);
}

QJsonValue AgaveTaskReply::retriveMainAgaveJSON(QJsonDocument parsedDoc, QList<QString> keyList)
{
    QJsonValue nullVal;
    if (keyList.size() < 1) return nullVal;
    if (parsedDoc.isNull()) return nullVal;
    if (parsedDoc.isEmpty()) return nullVal;
    if (!parsedDoc.isObject()) return nullVal;

    QJsonValue resultVal = recursiveJSONdig(QJsonValue(parsedDoc.object()) , &keyList, 0);
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
        ErrorPopup("Invalid JSON state");
        return nullValue;
    }

    //Get next obj
    if (!currVal.isObject()) return nullValue;
    if (!currVal.toObject().contains(keyList->at(i))) return nullValue;
    QJsonValue targetedValue = currVal.toObject().value(keyList->at(i));
    if (targetedValue.isUndefined()) return nullValue;

    if (i == keyList->size() - 1)
    {
        return targetedValue;
    }

    return recursiveJSONdig(targetedValue,keyList,i+1);
}
