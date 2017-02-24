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

#ifndef AGAVEHANDLER_H
#define AGAVEHANDLER_H

//Note: this interface is a work in progress. As the project develops, we should gain some insight into
//the various operations should fit together. (Particularly, what tasks should be "privlidged" and how.)

//TODO: Probably should rethink pass-by-val vs. pass-by-pointer, if it would save memory
//But many QT containers return copies by val, so think this thru carefully

#include <QtGlobal>
#include <QObject>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QJsonDocument>
#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo>

enum class AgaveState {NO_AUTH, GETTING_AUTH, LOGGED_IN, SHUTDOWN};
enum class RequestState {FAIL, GOOD, NO_CONNECT};
enum class AgaveRequestType {AGAVE_GET, AGAVE_POST, AGAVE_DELETE, AGAVE_UPLOAD, AGAVE_PUT};

class QNetworkReply;
class AgaveTaskGuide;
class VWTinterfaceDriver;
class AgaveTaskReply;

class AgaveHandler : public QObject
{
    Q_OBJECT

public:
    explicit AgaveHandler(VWTinterfaceDriver * theDriver);
    ~AgaveHandler();

    //Note: Authorization and directory listing are privlidged operations
    //    : and should have their signal connections created directly
    //Reason: File reading and authentication require auth data, which this object has
    void performAuth(QString uname, QString passwd);
    void retriveDirListing(QString dirPath);

    //Other requests must use an AgaveTaskReply object, and are only valid if
    //Authentication is already complete
    AgaveTaskReply * performAgaveQuery(QString queryName);
    AgaveTaskReply * performAgaveQuery(QString queryName, QStringList queryParams);
    AgaveTaskReply * performAgaveQuery(QString queryName, QStringList URLParams, QStringList postParams);

    void closeAllConnections();

    AgaveState getState();

signals:
    void stateChanged(AgaveState oldState, AgaveState newState);

    void sendAuthResult(RequestState resultState);
    void sendFileData(QJsonValue fileList);

private slots:
    void processPrivlidgedTask(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData);

private:
    void setupTaskGuideList();
    void insertAgaveTaskGuide(AgaveTaskGuide newGuide);

    AgaveTaskReply * makeAgaveRequest(QString urlAppend, AgaveRequestType type, QString taskName, QByteArray * authHeader = NULL, QByteArray postData = "", QFile * fileHandle = NULL);
    void changeState(AgaveState newState);

    void runSimpleTask(QString queryName);
    void runSimpleTask(QString queryName, QStringList queryParams);
    void runSimpleTask(QString queryName, QStringList URLParams, QStringList postParams);

    void clearAllAuthTokens();

    AgaveTaskGuide retriveTaskGuide(QString taskID);

    QNetworkAccessManager networkHandle;
    QSslConfiguration SSLoptions;
    const QString tenantURL = "https://agave.designsafe-ci.org";
    const QString clientName = "SimCenterWindGUI";
    const QString storageNode = "designsafe.storage.default";

    QByteArray authEncloded;
    QByteArray clientEncoded;
    QByteArray token;
    QByteArray tokenHeader;
    QByteArray refreshToken;

    QString authUname;
    QString authPass;
    QString clientKey;
    QString clientSecret;

    QMap<QString, AgaveTaskGuide> validTaskList;

    AgaveState currentState;
    VWTinterfaceDriver * mainDriver;
};

#endif // AGAVEHANDLER_H
