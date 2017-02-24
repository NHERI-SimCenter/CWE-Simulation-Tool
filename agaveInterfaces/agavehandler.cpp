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

#include "agavehandler.h"

#include "agavetaskguide.h"
#include "agavetaskreply.h"
#include "../vwtinterfacedriver.h"
#include "../programWindows/errorpopup.h"

AgaveHandler::AgaveHandler(VWTinterfaceDriver *theDriver) :
        QObject(0), networkHandle(0), SSLoptions()
{
    SSLoptions.setProtocol(QSsl::SecureProtocols);

    clearAllAuthTokens();

    currentState = AgaveState::NO_AUTH;

    mainDriver = theDriver;

    setupTaskGuideList();
}

void AgaveHandler::setupTaskGuideList()
{
    AgaveTaskGuide authStep1("authStep1", AgaveRequestType::AGAVE_GET);
    authStep1.addValidStartState(AgaveState::GETTING_AUTH);
    authStep1.addValidReplyState(AgaveState::GETTING_AUTH);
    authStep1.setURLsuffix(QString("/clients/v2/%1").arg(clientName));
    authStep1.setHeaderType(AuthHeaderType::PASSWD);
    authStep1.setOutputSigFail("message", {"message"});
    authStep1.setAsPrivlidged();
    insertAgaveTaskGuide(authStep1);

    AgaveTaskGuide authStep1a("authStep1a", AgaveRequestType::AGAVE_DELETE);
    authStep1a.addValidStartState(AgaveState::GETTING_AUTH);
    authStep1a.addValidReplyState(AgaveState::GETTING_AUTH);
    authStep1a.setURLsuffix(QString("/clients/v2/%1").arg(clientName));
    authStep1a.setHeaderType(AuthHeaderType::PASSWD);
    authStep1a.setAsPrivlidged();
    insertAgaveTaskGuide(authStep1a);

    AgaveTaskGuide authStep2("authStep2", AgaveRequestType::AGAVE_POST);
    authStep2.addValidStartState(AgaveState::GETTING_AUTH);
    authStep2.addValidReplyState(AgaveState::GETTING_AUTH);
    authStep2.setURLsuffix(QString("/clients/v2/"));
    authStep2.setHeaderType(AuthHeaderType::PASSWD);
    authStep2.setPostParams(QString("clientName=%1&description=Client ID for SimCenter Wind GUI App").arg(clientName),0);
    authStep2.setOutputSigGood("consumerKey", {"result", "consumerKey"});
    authStep2.setOutputSigGood("consumerSecret", {"result", "consumerSecret"});
    authStep2.setAsPrivlidged();
    insertAgaveTaskGuide(authStep2);

    AgaveTaskGuide authStep3("authStep3", AgaveRequestType::AGAVE_POST);
    authStep3.addValidStartState(AgaveState::GETTING_AUTH);
    authStep3.addValidReplyState(AgaveState::GETTING_AUTH);
    authStep3.setURLsuffix(QString("/token"));
    authStep3.setHeaderType(AuthHeaderType::CLIENT);
    authStep3.setPostParams("username=%1&password=%2&grant_type=password&scope=PRODUCTION",2);
    authStep3.setTokenFormat(true);
    authStep3.setOutputSigGood("access_token", {"access_token"});
    authStep3.setOutputSigGood("refresh_token", {"refresh_token"});
    authStep3.setAsPrivlidged();
    insertAgaveTaskGuide(authStep3);

    AgaveTaskGuide authRefresh("authRefresh", AgaveRequestType::AGAVE_POST);
    authRefresh.setURLsuffix(QString("/token"));
    authRefresh.setHeaderType(AuthHeaderType::CLIENT);
    authRefresh.setPostParams("grant_type=refresh_token&scope=PRODUCTION&refresh_token=%1",1);
    authRefresh.setTokenFormat(true);
    authRefresh.setOutputSigGood("access_token", {"access_token"});
    authRefresh.setOutputSigGood("refresh_token", {"refresh_token"});
    authRefresh.setAsPrivlidged();
    insertAgaveTaskGuide(authRefresh);

    AgaveTaskGuide authRevoke("authRevoke", AgaveRequestType::AGAVE_POST);
    authRevoke.addValidStartState(AgaveState::SHUTDOWN);
    authRevoke.addValidReplyState(AgaveState::SHUTDOWN);
    authRevoke.setURLsuffix(QString("/revoke"));
    authRevoke.setHeaderType(AuthHeaderType::CLIENT);
    authRevoke.setPostParams("token=%1",1);
    authRevoke.setAsPrivlidged();
    insertAgaveTaskGuide(authRevoke);

    AgaveTaskGuide dirListing("dirListing", AgaveRequestType::AGAVE_GET);
    dirListing.setURLsuffix((QString("/files/v2/listings/system/%1/")).arg(storageNode));
    dirListing.setDynamicURLParams("%1",1);
    dirListing.setHeaderType(AuthHeaderType::TOKEN);
    dirListing.setOutputSigGood("fileList", {"result"});
    dirListing.setAsPrivlidged();
    insertAgaveTaskGuide(dirListing);

    AgaveTaskGuide fileUpload("fileUpload", AgaveRequestType::AGAVE_UPLOAD);
    fileUpload.setURLsuffix((QString("/files/v2/media/system/%1/")).arg(storageNode));
    fileUpload.setDynamicURLParams("%1",1);
    fileUpload.setPostParams("%1",1); //AGAVE_UPLOAD always uses a single parameter for the full file name
    fileUpload.setHeaderType(AuthHeaderType::TOKEN);
    fileUpload.setOutputSigGood("result", {"result"});
    insertAgaveTaskGuide(fileUpload);

    AgaveTaskGuide fileDelete("fileDelete", AgaveRequestType::AGAVE_DELETE);
    fileDelete.setURLsuffix((QString("/files/v2/media/system/%1/")).arg(storageNode));
    fileDelete.setDynamicURLParams("%1",1);
    fileDelete.setHeaderType(AuthHeaderType::TOKEN);
    fileDelete.setOutputSigGood("result", {"result"});
    insertAgaveTaskGuide(fileDelete);

    AgaveTaskGuide newFolder("newFolder", AgaveRequestType::AGAVE_PUT);
    newFolder.setURLsuffix((QString("/files/v2/media/system/%1/")).arg(storageNode));
    newFolder.setDynamicURLParams("%1",1);
    newFolder.setPostParams("action=mkdir&path=%1",1);
    newFolder.setHeaderType(AuthHeaderType::TOKEN);
    newFolder.setOutputSigGood("result", {"result"});
    insertAgaveTaskGuide(newFolder);

    AgaveTaskGuide renameFile("renameFile", AgaveRequestType::AGAVE_PUT);
    renameFile.setURLsuffix((QString("/files/v2/media/system/%1/")).arg(storageNode));
    renameFile.setDynamicURLParams("%1",1);
    renameFile.setPostParams("action=rename&path=%1",1);
    renameFile.setHeaderType(AuthHeaderType::TOKEN);
    renameFile.setOutputSigGood("result", {"result"});
    insertAgaveTaskGuide(renameFile);
}

void AgaveHandler::insertAgaveTaskGuide(AgaveTaskGuide newGuide)
{
    QString taskName = newGuide.getTaskID();
    if (validTaskList.contains(taskName))
    {
        ErrorPopup("Invalid Task Guide List: Duplicate Name");
        return;
    }
    validTaskList.insert(taskName,newGuide);
}

AgaveHandler::~AgaveHandler()
{
    closeAllConnections();
}

void AgaveHandler::closeAllConnections()
{
    AgaveState prevState = currentState;
    qDebug("Shutdown sequence begins");
    changeState(AgaveState::SHUTDOWN);
    if ((prevState == AgaveState::LOGGED_IN) && (clientEncoded != "") && (token != ""))
    {
        runSimpleTask(QString("authRevoke"), QList<QString>{token});
    }
    //Remove client entry?
}

AgaveState AgaveHandler::getState()
{
    return currentState;
}

void AgaveHandler::processPrivlidgedTask(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    if (resultState == RequestState::NO_CONNECT)
    {
        if (currentState == AgaveState::GETTING_AUTH)
        {
            changeState(AgaveState::NO_AUTH);
            clearAllAuthTokens();
            emit sendAuthResult(RequestState::NO_CONNECT);
        }
        else
        {
            //TODO: This should probably have its own error number
            ErrorPopup("Unable to connect to agave server. Check network connection and try again");
        }
        return;
    }

    if (taskID == "authStep1")
    {
        if (resultState == RequestState::GOOD)
        {
            runSimpleTask("authStep1a");
        }
        else
        {
            if (rawData.value("message").toString() == "Application not found")
            {
                runSimpleTask("authStep2");
            }
            else if (rawData.value("message").toString() == "Login failed.Please recheck the username and password and try again.")
            {
                changeState(AgaveState::NO_AUTH);
                clearAllAuthTokens();
                emit sendAuthResult(RequestState::FAIL);
            }
            else
            {
                changeState(AgaveState::NO_AUTH);
                clearAllAuthTokens();
                emit sendAuthResult(RequestState::NO_CONNECT);
            }
        }
    }
    else if (taskID == "authStep1a")
    {
        if (resultState == RequestState::GOOD)
        {
            runSimpleTask("authStep2");
        }
        else
        {
            changeState(AgaveState::NO_AUTH);
            clearAllAuthTokens();
            emit sendAuthResult(RequestState::FAIL);
        }
    }
    else if (taskID == "authStep2")
    {
        if (resultState == RequestState::GOOD)
        {
            if (rawData.value("consumerKey").toString().isEmpty() ||
                rawData.value("consumerSecret").toString().isEmpty())
            {
                ErrorPopup("Client success fails.");
                return;
            }
            clientKey = rawData.value("consumerKey").toString();
            clientSecret = rawData.value("consumerSecret").toString();

            clientEncoded = "Basic ";
            QByteArray rawAuth(clientKey.toLatin1());
            rawAuth.append(":");
            rawAuth.append(clientSecret);
            clientEncoded.append(rawAuth.toBase64());

            runSimpleTask("authStep3", QList<QString>({authUname, authPass}));
        }
        else
        {
            changeState(AgaveState::NO_AUTH);
            clearAllAuthTokens();
            emit sendAuthResult(RequestState::FAIL);
        }
    }
    else if (taskID == "authStep3")
    {
        if (resultState == RequestState::GOOD)
        {
            if (rawData.value("access_token").toString().isEmpty() ||
                rawData.value("refresh_token").toString().isEmpty())
            {
                changeState(AgaveState::NO_AUTH);
                clearAllAuthTokens();
                emit sendAuthResult(RequestState::FAIL);
            }
            else
            {
                token = rawData.value("access_token").toString().toLatin1();
                tokenHeader = (QString("Bearer ").append(token)).toLatin1();
                refreshToken = rawData.value("refresh_token").toString().toLatin1();

                changeState(AgaveState::LOGGED_IN);
                emit sendAuthResult(RequestState::GOOD);
                qDebug("Login success.");
            }
        }
        else
        {
            changeState(AgaveState::NO_AUTH);
            clearAllAuthTokens();
            emit sendAuthResult(RequestState::FAIL);
        }
    }
    else if (taskID == "authRefresh")
    {
        if (resultState == RequestState::GOOD)
        {
            if (rawData.value("access_token").toString().isEmpty() ||
                rawData.value("refresh_token").toString().isEmpty())
            {
                ErrorPopup("Token refresh failure.");
                return;
            }
            token = rawData.value("access_token").toString().toLatin1();
            refreshToken = rawData.value("refresh_token").toString().toLatin1();
        }
    }
    else if (taskID == "authRevoke")
    {
        qDebug("Token Revoke sucess?");
    }
    else if (taskID == "dirListing")
    {
        if ((resultState == RequestState::GOOD) && !rawData.isEmpty() && rawData.contains("fileList"))
        {
            emit sendFileData(rawData.value("fileList"));
        }
        else
        {
            qDebug("Listing Failed");
            //TODO: Probably should have some more meaningful error handleing here
        }
    }
    else
    {
        ErrorPopup("Non-existant privlidged request requested.");
    }
}

void AgaveHandler::changeState(AgaveState newState)
{
    AgaveState oldState = currentState;
    currentState = newState;
    emit stateChanged(oldState, newState);
}

void AgaveHandler::performAuth(QString uname, QString passwd)
{
    if (currentState != AgaveState::NO_AUTH)
    {
        return;
    }
    changeState(AgaveState::GETTING_AUTH);

    authUname = uname;
    authPass = passwd;

    authEncloded = "Basic ";
    QByteArray rawAuth(uname.toLatin1());
    rawAuth.append(":");
    rawAuth.append(passwd);
    authEncloded.append(rawAuth.toBase64());

    runSimpleTask("authStep1");
}

void AgaveHandler::retriveDirListing(QString dirPath)
{
    if (dirPath == "/")
    {
        dirPath.append(authUname);
    }
    runSimpleTask("dirListing", QList<QString>({dirPath}));
}

void AgaveHandler::runSimpleTask(QString queryName)
{
    QStringList emptyList;
    runSimpleTask(queryName, emptyList, emptyList);
}

void AgaveHandler::runSimpleTask(QString queryName, QStringList queryParams)
{
    QStringList emptyList;
    AgaveTaskGuide taskGuide = retriveTaskGuide(queryName);
    if (taskGuide.needsOnlyPostParms())
    {
        runSimpleTask(queryName,emptyList,queryParams);
    }

    if (taskGuide.needsOnlyURLParams())
    {
        runSimpleTask(queryName,queryParams,emptyList);
    }
    //TODO: This should be an error
}

void AgaveHandler::runSimpleTask(QString queryName, QStringList URLParams, QStringList postParams)
{
    //TODO: should plroabaly check if the task is privlidged here
    AgaveTaskReply * replyToLink = performAgaveQuery(queryName, URLParams, postParams);
    if (replyToLink == NULL)
    {
        qDebug("Prividged Task Failed");
        return;
    }
    QObject::connect(replyToLink, SIGNAL(sendAgaveResultData(QString,RequestState,QMap<QString,QJsonValue>)),
                     this, SLOT(processPrivlidgedTask(QString,RequestState,QMap<QString,QJsonValue>)));
}

AgaveTaskReply * AgaveHandler::performAgaveQuery(QString queryName)
{
    QStringList emptyList;
    return performAgaveQuery(queryName, emptyList, emptyList);
}

AgaveTaskReply * AgaveHandler::performAgaveQuery(QString queryName, QStringList queryParams)
{
    QStringList emptyList;
    AgaveTaskGuide taskGuide = retriveTaskGuide(queryName);
    if (taskGuide.needsOnlyPostParms())
    {
        return performAgaveQuery(queryName,emptyList,queryParams);
    }

    if (taskGuide.needsOnlyURLParams())
    {
        return performAgaveQuery(queryName,queryParams,emptyList);
    }
    //TODO: This should be an error
    return NULL;
}

AgaveTaskReply * AgaveHandler::performAgaveQuery(QString queryName, QStringList URLParams, QStringList postParams)
{
    AgaveTaskGuide taskGuide = retriveTaskGuide(queryName);

    if (!taskGuide.verifyValidStartState(currentState))
    {
        return NULL;
    }

    QByteArray * authHeader = NULL;
    if (taskGuide.getHeaderType() == AuthHeaderType::CLIENT)
    {
        authHeader = &clientEncoded;
    }
    else if (taskGuide.getHeaderType() == AuthHeaderType::PASSWD)
    {
        authHeader = &authEncloded;
    }
    else if (taskGuide.getHeaderType() == AuthHeaderType::REFRESH)
    {
        //TODO
    }
    else if (taskGuide.getHeaderType() == AuthHeaderType::TOKEN)
    {
        authHeader = &tokenHeader;
    }

    if (taskGuide.getRequestType() == AgaveRequestType::AGAVE_POST)
    {
        QByteArray clientPostData;
        if (postParams.isEmpty())
        {
            clientPostData = taskGuide.fillPostArgList();
        }
        else
        {
            clientPostData = taskGuide.fillPostArgList(&postParams);
        }
        qDebug("Post data: %s", clientPostData.toStdString().c_str());

        return makeAgaveRequest(taskGuide.getURLsuffix(),
                         taskGuide.getRequestType(),
                         queryName,
                         authHeader, clientPostData);
    }
    else if ((taskGuide.getRequestType() == AgaveRequestType::AGAVE_GET) || (taskGuide.getRequestType() == AgaveRequestType::AGAVE_DELETE))
    {
        QByteArray realURLsuffix = taskGuide.getURLsuffix().toLatin1();
        if (URLParams.isEmpty())
        {
            realURLsuffix.append(taskGuide.fillURLArgList());
        }
        else
        {
            realURLsuffix.append(taskGuide.fillURLArgList(&URLParams));
        }
        qDebug("URL Req: %s", realURLsuffix.toStdString().c_str());

        return makeAgaveRequest(realURLsuffix,
                         taskGuide.getRequestType(),
                         queryName,
                         authHeader);
    }
    else if (taskGuide.getRequestType() == AgaveRequestType::AGAVE_UPLOAD)
    {
        //Upload is a single post parameter
        if (postParams.length() != 1) return NULL;
        QString fullFileName = postParams.at(0);
        QFile * fileHandle = new QFile(fullFileName);
        if (!fileHandle->open(QIODevice::ReadOnly))
        {
            fileHandle->deleteLater();
            return NULL;
        }

        QByteArray realURLsuffix = taskGuide.getURLsuffix().toLatin1();
        if (URLParams.isEmpty())
        {
            realURLsuffix.append(taskGuide.fillURLArgList());
        }
        else
        {
            realURLsuffix.append(taskGuide.fillURLArgList(&URLParams));
        }
        qDebug("URL Req: %s", realURLsuffix.toStdString().c_str());

        QByteArray emptyPostData;

        return makeAgaveRequest(realURLsuffix,
                         taskGuide.getRequestType(),
                         queryName,
                         authHeader, emptyPostData, fileHandle);
    }
    else if (taskGuide.getRequestType() == AgaveRequestType::AGAVE_PUT)
    {
        QByteArray clientPutData;
        if (postParams.isEmpty())
        {
            clientPutData = taskGuide.fillPostArgList();
        }
        else
        {
            clientPutData = taskGuide.fillPostArgList(&postParams);
        }

        QByteArray realURLsuffix = taskGuide.getURLsuffix().toLatin1();
        if (URLParams.isEmpty())
        {
            realURLsuffix.append(taskGuide.fillURLArgList());
        }
        else
        {
            realURLsuffix.append(taskGuide.fillURLArgList(&URLParams));
        }
        qDebug("URL Req: %s", realURLsuffix.toStdString().c_str());

        return makeAgaveRequest(realURLsuffix,
                         taskGuide.getRequestType(),
                         queryName,
                         authHeader, clientPutData);
    }
    else
    {
        //TODO: Probably should have fatal error here
        return NULL;
    }

    return NULL;
}

AgaveTaskReply *AgaveHandler::makeAgaveRequest(QString urlAppend, AgaveRequestType type, QString taskName, QByteArray * authHeader, QByteArray postData, QFile * fileHandle)
{
    QNetworkReply * clientReply = NULL;
    AgaveTaskReply * ret = NULL;

    QString activeURL = tenantURL;
    activeURL.append(urlAppend);

    QNetworkRequest * clientRequest = new QNetworkRequest();
    clientRequest->setUrl(QUrl(activeURL));

    //clientRequest->setRawHeader("User-Agent", "SimCenterWindGUI");
    if (type == AgaveRequestType::AGAVE_POST)
    {
        //TODO: this gave us problems before, defults are needed for multiparts, but POST should be set manually
        //Should look into this to make sure it is fully correct
        clientRequest->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    }
    //else if (type == AgaveRequestType::AGAVE_JSON_POST)
    //{
    //    clientRequest->setHeader(QNetworkRequest::ContentTypeHeader, "Content-Type: application/json");
    //}

    if (authHeader != NULL)
    {
        if (authHeader->isEmpty())
        {
            ErrorPopup(VWTerrorType::ERR_AUTH_BLANK);
            return ret;
        }
        clientRequest->setRawHeader(QByteArray("Authorization"), *authHeader);
    }

    //Note: to suppress SSL warning for not having obsolete SSL versions, use
    // QT_LOGGING_RULES in the project build environment variables. Set to:
    // qt.network.ssl.warning=false
    clientRequest->setSslConfiguration(SSLoptions);

    qDebug("%s", clientRequest->url().url().toStdString().c_str());

    if (type == AgaveRequestType::AGAVE_GET)
    {
        clientReply = networkHandle.get(*clientRequest);
    }
    else if (type == AgaveRequestType::AGAVE_POST)
    {
        clientReply = networkHandle.post(*clientRequest, postData);
    }
    else if (type == AgaveRequestType::AGAVE_PUT)
    {
        clientReply = networkHandle.put(*clientRequest, postData);
    }
    else if (type == AgaveRequestType::AGAVE_DELETE)
    {
        clientReply = networkHandle.deleteResource(*clientRequest);
    }
    else if (type == AgaveRequestType::AGAVE_UPLOAD)
    {
        QHttpMultiPart * fileUpload = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart filePart;
        filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-strem"));
        QString tempString = "form-data; name=\"fileToUpload\"; filename=\"%1\"";
        QFileInfo fileInfo(fileHandle->fileName());
        tempString = tempString.arg(fileInfo.fileName());
        filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(tempString));

        filePart.setBodyDevice(fileHandle);
        fileHandle->setParent(fileUpload);

        fileUpload->append(filePart);

        clientReply = networkHandle.post(*clientRequest, fileUpload);
        fileUpload->setParent(clientReply);
    }

    if (clientReply == NULL)
    {
        ErrorPopup("Non-existant request requested.2");
        return ret;
    }

    ret = new AgaveTaskReply(this,retriveTaskGuide(taskName),clientReply);

    if (!ret->isValidRequest())
    {
        ErrorPopup("Request Results in NULL");
        ret->deleteLater();
        return NULL;
    }

    if (networkHandle.networkAccessible() != QNetworkAccessManager::Accessible)
    {
        ErrorPopup("Network not available");
        ret->deleteLater();
        return NULL;
    }
    return ret;
}

void AgaveHandler::clearAllAuthTokens()
{
    authEncloded = "";
    clientEncoded = "";
    token = "";
    refreshToken = "";

    authUname = "";
    authPass = "";
    clientKey = "";
    clientSecret = "";
}

AgaveTaskGuide AgaveHandler::retriveTaskGuide(QString taskID)
{
    AgaveTaskGuide ret;
    if (!validTaskList.contains(taskID))
    {
        ErrorPopup("Non-existant request requested.1");
        return ret;
    }
    ret = validTaskList.value(taskID);
    if (taskID != ret.getTaskID())
    {
        ErrorPopup("Task Guide format error.");
    }
    return ret;
}
