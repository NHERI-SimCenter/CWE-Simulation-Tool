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

#include "agavetaskguide.h"
#include "agavehandler.h"

AgaveTaskGuide::AgaveTaskGuide()
{
    taskId = "INVALID";
}

AgaveTaskGuide::AgaveTaskGuide(QString newID, AgaveRequestType reqType)
{
    taskId = newID;
    requestType = reqType;

    if ((requestType == AgaveRequestType::AGAVE_UPLOAD) || (requestType == AgaveRequestType::AGAVE_DOWNLOAD))
    {
        //Agave Upload takes one param: the full file name
        //Agave Download takes one param: the local destination name
        setPostParams("%1", 1);
    }
}

QString AgaveTaskGuide::getTaskID()
{
    return taskId;
}

QString AgaveTaskGuide::getURLsuffix()
{
    return URLsuffix;
}

AgaveRequestType AgaveTaskGuide::getRequestType()
{
    return requestType;
}

AuthHeaderType AgaveTaskGuide::getHeaderType()
{
    return headerType;
}

bool AgaveTaskGuide::isTokenFormat()
{
    return usesTokenFormat;
}

void AgaveTaskGuide::setAsInternal()
{
    internalTask = true;
}

bool AgaveTaskGuide::isInternal()
{
    return internalTask;
}

QByteArray AgaveTaskGuide::fillPostArgList(QStringList * argList)
{
    return fillAnyArgList(argList, numPostVals, postFormat);
}

QByteArray AgaveTaskGuide::fillURLArgList(QStringList * argList)
{
    return fillAnyArgList(argList, numDynURLVals, dynURLFormat);
}

QByteArray AgaveTaskGuide::fillAnyArgList(QStringList * argList, int numVals, QString strFormat)
{
    if ((argList == NULL) || (numVals != argList->size()) || (numVals == 0))
    {
        return strFormat.toLatin1();
    }

    QString ret = strFormat;

    for (auto itr = argList->cbegin(); itr != argList->cend(); ++itr)
    {
        ret = ret.arg(*itr);
    }
    return ret.toLatin1();
}

void AgaveTaskGuide::setURLsuffix(QString newValue)
{
    URLsuffix = newValue;
}

void AgaveTaskGuide::setHeaderType(AuthHeaderType newValue)
{
    headerType = newValue;
}

void AgaveTaskGuide::setTokenFormat(bool newSetting)
{
    usesTokenFormat = newSetting;
}

void AgaveTaskGuide::setDynamicURLParams(QString format, int numSubs)
{
    needsURLParams = true;
    dynURLFormat = format;
    numDynURLVals = numSubs;
}

void AgaveTaskGuide::setPostParams(QString format, int numSubs)
{
    needsPostParams = true;
    postFormat = format;
    numPostVals = numSubs;
}

void AgaveTaskGuide::setStoreParam(int paramList, int elementToStore)
{
    storeParamInReply = true;
    storeParamList = paramList;
    storeParamElement = elementToStore;
}

bool AgaveTaskGuide::hasStoredParam()
{
    return storeParamInReply;
}

int AgaveTaskGuide::getStoredParamList()
{
    return storeParamList;
}

int AgaveTaskGuide::getStoredParamElement()
{
    return storeParamElement;
}

bool AgaveTaskGuide::usesPostParms()
{
    return needsPostParams;
}

bool AgaveTaskGuide::usesURLParams()
{
    return needsURLParams;
}
