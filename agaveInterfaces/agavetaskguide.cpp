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

//TODO: Strightforward copy constructor?
//TODO: should probably overload asssignment operator too? Maybe not needed

AgaveTaskGuide::AgaveTaskGuide(QString newID, AgaveRequestType reqType)
{
    taskId = newID;
    requestType = reqType;

    validStartStates.append(AgaveState::LOGGED_IN);
    validReplyStates.append(AgaveState::LOGGED_IN);
    usesDefaultValidStates = true;
}

QString AgaveTaskGuide::getTaskID()
{
    return taskId;
}

bool AgaveTaskGuide::verifyValidStartState(AgaveState checkState)
{
    return verifiyValidState(checkState, &validStartStates);
}

bool AgaveTaskGuide::verifyValidReplyState(AgaveState checkState)
{
    return verifiyValidState(checkState, &validReplyStates);
}

bool AgaveTaskGuide::verifiyValidState(AgaveState checkState, QList<AgaveState> * stateList)
{
    return stateList->contains(checkState);
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

QMap<QString, QList<QString>> AgaveTaskGuide::getOutputSignatureGood()
{
    return outputsOfInterestGood;
}

QMap<QString, QList<QString>> AgaveTaskGuide::getOutputSignatureFail()
{
    return outputsOfInterestFail;
}

bool AgaveTaskGuide::isTokenFormat()
{
    return usesTokenFormat;
}

QByteArray AgaveTaskGuide::fillPostArgList()
{
    return fillAnyArgList(NULL, numPostVals, postFormat);
}

QByteArray AgaveTaskGuide::fillPostArgList(QStringList * argList)
{
    return fillAnyArgList(argList, numPostVals, postFormat);
}

QByteArray AgaveTaskGuide::fillURLArgList()
{
    return fillAnyArgList(NULL, numDynURLVals, dynURLFormat);
}

QByteArray AgaveTaskGuide::fillURLArgList(QStringList * argList)
{
    return fillAnyArgList(argList, numDynURLVals, dynURLFormat);
}

QByteArray AgaveTaskGuide::fillAnyArgList(QStringList * argList, int numVals, QString strFormat)
{
    if ((argList == NULL) || (argList->size() == 0))
    {
        if (numVals != 0)
        {
            return "";
        }
        return strFormat.toLatin1();
    }
    if (numVals != argList->size())
    {
        return "";
    }

    QString ret = strFormat;

    for (auto itr = argList->cbegin(); itr != argList->cend(); ++itr)
    {
        ret = ret.arg(*itr);
    }
    return ret.toLatin1();
}

bool AgaveTaskGuide::isPrivlidged()
{
    return privlidgedTask;
}

void AgaveTaskGuide::addValidStartState(AgaveState newState)
{
    if (usesDefaultValidStates == true)
    {
        usesDefaultValidStates = false;
        validStartStates.clear();
        validReplyStates.clear();
    }

    validStartStates.append(newState);
}

void AgaveTaskGuide::addValidReplyState(AgaveState newState)
{
    if (usesDefaultValidStates == true)
    {
        usesDefaultValidStates = false;
        validStartStates.clear();
        validReplyStates.clear();
    }

    validReplyStates.append(newState);
}

void AgaveTaskGuide::setURLsuffix(QString newValue)
{
    URLsuffix = newValue;
}

void AgaveTaskGuide::setHeaderType(AuthHeaderType newValue)
{
    headerType = newValue;
}

void AgaveTaskGuide::setPostParams(QString format, int numSubs)
{
    needsPostParams = true;
    postFormat = format;
    numPostVals = numSubs;
}

void AgaveTaskGuide::setOutputSigGood(QString outID, QStringList parseFindList)
{
    outputsOfInterestGood.insert(outID,parseFindList);
}

void AgaveTaskGuide::setOutputSigFail(QString outID, QStringList parseFindList)
{
    outputsOfInterestFail.insert(outID,parseFindList);
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

void AgaveTaskGuide::setAsPrivlidged()
{
    privlidgedTask = true;
}

bool AgaveTaskGuide::needsNoParams()
{
    return ((needsPostParams == false) && (needsURLParams == false));
}

bool AgaveTaskGuide::needsBothParams()
{
    return ((needsPostParams == true) && (needsURLParams == true));
}

bool AgaveTaskGuide::needsOnlyPostParms()
{
    return ((needsPostParams == true) && (needsURLParams == false));
}

bool AgaveTaskGuide::needsOnlyURLParams()
{
    return ((needsPostParams == false) && (needsURLParams == true));
}
