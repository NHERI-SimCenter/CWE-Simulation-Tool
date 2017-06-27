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

#include "vwtinterfacedriver.h"

#include "../AgaveClientInterface/agaveInterfaces/agavehandler.h"
#include "utilWindows/authform.h"
#include "utilWindows/errorpopup.h"
#include "utilWindows/quickinfopopup.h"
#include "utilWindows/debugpanelwindow.h"

#include "mainWindow/cwe_mainwindow.h"

VWTinterfaceDriver::VWTinterfaceDriver()
{
    AgaveHandler * tmpHandle = new AgaveHandler(this);
    tmpHandle->registerAgaveAppInfo("compress", "compress-0.1u1",{"directory", "compression_type"},{},"directory");
    tmpHandle->registerAgaveAppInfo("extract", "extract-0.1u1",{"inputFile"},{},"");
    tmpHandle->registerAgaveAppInfo("openfoam","openfoam-2.4.0u11",{"solver"},{"inputDirectory"},"inputDirectory");

    //The following are being debuged:
    tmpHandle->registerAgaveAppInfo("FileEcho", "fileEcho-0.1.0",{"directory","NewFile", "EchoText"},{},"directory");
    tmpHandle->registerAgaveAppInfo("PythonTest", "pythonRun-0.1.0",{"directory","NewFile"},{},"directory");
    tmpHandle->registerAgaveAppInfo("SectionMesh", "sectionMesh-0.1.0",{"SlicePlane"},{"directory","SGFFile","SimParams"},"SGFFile");
    tmpHandle->registerAgaveAppInfo("tempCFD","tempCFD-2.4.0",{"solver"},{"inputDirectory"},"inputDirectory");

    tmpHandle->registerAgaveAppInfo("twoDslice", "twoDslice-0.1.0", {"SlicePlane", "SimParams", "NewCaseFolder"},{"SGFFile"}, "SGFFile");
    tmpHandle->registerAgaveAppInfo("twoDUmesh", "twoDUmesh-0.1.0", {"MeshParams","directory"},{}, "directory");

    theConnector = (RemoteDataInterface *) tmpHandle;
    authWindow = NULL;
    mainWindow = NULL;

    QObject::connect(theConnector, SIGNAL(sendFatalErrorMessage(QString)), this, SLOT(getFatalInterfaceError(QString)));
}

VWTinterfaceDriver::~VWTinterfaceDriver()
{
    if (theConnector != NULL) theConnector->deleteLater();
    if (authWindow != NULL) authWindow->deleteLater();
    if (mainWindow != NULL) mainWindow->deleteLater();
    if (debugWindow != NULL) debugWindow->deleteLater();
}

void VWTinterfaceDriver::startup(bool useDebugPanel)
{
    authWindow = new AuthForm(theConnector, this);
    authWindow->show();
    QObject::connect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    mainWindow = new CWE_MainWindow(this);

    if (useDebugPanel)
    {
        debugWindow = new DebugPanelWindow(theConnector);
    }
}

void VWTinterfaceDriver::shutdown()
{
    if (doingShutdown)
    {
        return;
    }
    doingShutdown = true;
    qDebug("Beginning graceful shutdown.");
    if (theConnector != NULL)
    {
        RemoteDataReply * revokeTask = theConnector->closeAllConnections();
        if (revokeTask != NULL)
        {
            QObject::connect(revokeTask, SIGNAL(connectionsClosed(RequestState)), this, SLOT(shutdownCallback()));
            qDebug("Waiting on outstanding tasks");
            QuickInfoPopup * waitOnCloseMessage = new QuickInfoPopup("Waiting for network shutdown. Click OK to force quit.");
            QObject::connect(waitOnCloseMessage, SIGNAL(accepted()), this, SLOT(shutdownCallback()));
            waitOnCloseMessage->show();
            return;
        }
    }
    shutdownCallback();
}

void VWTinterfaceDriver::shutdownCallback()
{
    QCoreApplication::instance()->exit(0);
}

void VWTinterfaceDriver::subWindowHidden(bool nowVisible)
{
    if (nowVisible == false)
    {
        shutdown();
    }
}

RemoteDataInterface * VWTinterfaceDriver::getDataConnection()
{
    return theConnector;
}

void VWTinterfaceDriver::getAuthReply(RequestState authReply)
{
    if ((authReply == RequestState::GOOD) && (authWindow != NULL) && (authWindow->isVisible()))
    {
        closeAuthScreen();
    }
}

void VWTinterfaceDriver::getFatalInterfaceError(QString errText)
{
    ErrorPopup((QString)errText);
}

void VWTinterfaceDriver::closeAuthScreen()
{
    if (mainWindow == NULL)
    {
        ErrorPopup anError(VWTerrorType::ERR_WINDOW_SYSTEM);
        return;
    }
    //ErrorPopup("This is a test of the error popup");

    mainWindow->show();

    //The dynamics of this are different in windows. TODO: Find a more cross-platform solution
    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    if (authWindow != NULL)
    {
        QObject::disconnect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
        authWindow->hide();
        authWindow->deleteLater();
        authWindow = NULL;
    }

    if (debugWindow != NULL)
    {
        debugWindow->startAndShow();
    }
}
