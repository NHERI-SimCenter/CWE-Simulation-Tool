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

#ifndef AGAVETASKGUIDE_H
#define AGAVETASKGUIDE_H

#include <QMap>
#include <QString>
#include <QStringList>

enum class AgaveRequestType;
enum class AgaveState;

enum class AuthHeaderType {NONE, PASSWD, CLIENT, TOKEN, REFRESH};

class AgaveTaskGuide
{
public:
    explicit AgaveTaskGuide();
    explicit AgaveTaskGuide(QString newID, AgaveRequestType reqType);

    void addValidStartState(AgaveState newState);
    void addValidReplyState(AgaveState newState);

    void setURLsuffix(QString newValue);
    void setHeaderType(AuthHeaderType newValue);
    void setPostParams(QString format, int numSubs);
    void setOutputSigGood(QString outID, QStringList parseFindList);
    void setOutputSigFail(QString outID, QStringList parseFindList);
    void setTokenFormat(bool newSetting);
    void setDynamicURLParams(QString format, int numSubs);
    void setAsPrivlidged();

    QString getTaskID();
    bool verifyValidStartState(AgaveState checkState);
    bool verifyValidReplyState(AgaveState checkState);
    QString getURLsuffix();
    AgaveRequestType getRequestType();
    AuthHeaderType getHeaderType();
    QByteArray fillPostArgList(QStringList *argList);
    QByteArray fillPostArgList();
    QMap<QString, QList<QString>> getOutputSignatureGood();
    QMap<QString, QList<QString>> getOutputSignatureFail();
    QByteArray fillURLArgList(QStringList * argList);
    QByteArray fillURLArgList();
    bool isTokenFormat();
    bool isPrivlidged();

    bool needsNoParams();
    bool needsBothParams();
    bool needsOnlyPostParms();
    bool needsOnlyURLParams();

private:
    QString taskId;
    QList<AgaveState> validStartStates;
    QList<AgaveState> validReplyStates;

    QString URLsuffix = "";
    AgaveRequestType requestType;
    AuthHeaderType headerType = AuthHeaderType::NONE;
    QMap<QString, QList<QString>> outputsOfInterestGood;
    QMap<QString, QList<QString>> outputsOfInterestFail;

    QByteArray fillAnyArgList(QStringList *argList, int numVals, QString strFormat);

    bool usesTokenFormat = false;
    bool privlidgedTask = false;
    bool usesDefaultValidStates;
    bool needsPostParams = false;
    bool needsURLParams = false;

    QString postFormat = "";
    int numPostVals = 0;

    QString dynURLFormat = "";
    int numDynURLVals = 0;
    bool verifiyValidState(AgaveState checkState, QList<AgaveState> * stateList);
};

#endif // AGAVETASKGUIDE_H
