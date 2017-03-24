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

using namespace std;

#include "vwtinterfacedriver.h"

#include "../AgaveClientInterface/agaveInterfaces/agavehandler.h"
#include "programWindows/authform.h"
#include "programWindows/panelwindow.h"
#include "programWindows/errorpopup.h"
#include "programWindows/quickinfopopup.h"

VWTinterfaceDriver::VWTinterfaceDriver()
{
    theConnector = (RemoteDataInterface *) (new AgaveHandler(this));
    authWindow = NULL;
    mainWindow = NULL;

    QObject::connect(theConnector, SIGNAL(sendFatalErrorMessage(QString)), this, SLOT(getFatalInterfaceError(QString)));
}

VWTinterfaceDriver::~VWTinterfaceDriver()
{
    if (theConnector != NULL) theConnector->deleteLater();
    if (authWindow != NULL) authWindow->deleteLater();
    if (mainWindow != NULL) mainWindow->deleteLater();
}

void VWTinterfaceDriver::startup()
{
    authWindow = new AuthForm(theConnector, this);
    QObject::connect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
    mainWindow = new PanelWindow(this);
    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    authWindow->show();
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
        ErrorPopup(VWTerrorType::ERR_WINDOW_SYSTEM);
        return;
    }
    //ErrorPopup("This is a test of the error popup");

    mainWindow->setupTaskList();
    mainWindow->show();

    if (authWindow != NULL)
    {
        QObject::disconnect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
        authWindow->hide();
        authWindow->deleteLater();
        authWindow = NULL;
    }
}
