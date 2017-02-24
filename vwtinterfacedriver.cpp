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

#include "agaveInterfaces/agavehandler.h"
#include "programWindows/authform.h"
#include "programWindows/panelwindow.h"
#include "programWindows/errorpopup.h"

VWTinterfaceDriver::VWTinterfaceDriver()
{
    agaveConnector = new AgaveHandler(this);
    authWindow = NULL;
    mainWindow = NULL;
    errorWindow = NULL;

    //TODO: Should probably have more dignified way to store this text.
    copyrightInfoText.append("The SimCenter Agave Client Program, in source and binary forms, is copyright \"The University of Notre Dame\" and \"The Regents of the University of California\" and is licensed under the following copyright and license:\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("BSD 3-Clause License");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("Copyright (c) 2017, The University of Notre Dame\n");
    copyrightInfoText.append("Copyright (c) 2017, The Regents of the University of California\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("All rights reserved.\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("Redistribution and use in source and binary forms, with or without\n");
    copyrightInfoText.append("modification, are permitted provided that the following conditions are met:\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("1. Redistributions of source code must retain the above copyright notice, this\n");
    copyrightInfoText.append("list of conditions and the following disclaimer.\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("2. Redistributions in binary form must reproduce the above copyright notice,\n");
    copyrightInfoText.append("this list of conditions and the following disclaimer in the documentation\n");
    copyrightInfoText.append("and/or other materials provided with the distribution.\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("3. Neither the name of the copyright holder nor the names of its\n");
    copyrightInfoText.append("contributors may be used to endorse or promote products derived from\n");
    copyrightInfoText.append("this software without specific prior written permission.\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"\n");
    copyrightInfoText.append("AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE\n");
    copyrightInfoText.append("IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n");
    copyrightInfoText.append("DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE\n");
    copyrightInfoText.append("FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL\n");
    copyrightInfoText.append("DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR\n");
    copyrightInfoText.append("SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n");
    copyrightInfoText.append("CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,\n");
    copyrightInfoText.append("OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n");
    copyrightInfoText.append("OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("------------------------------------------------------------------------------------\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("The SimCenter Agave Client Program makes use of the QT packages (unmodified): core, gui and network\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("QT is copyright \"The Qt Company Ltd\" and licensed under the GNU Lesser General Public License (version 3) which references the GNU General Public License (version 3)\n");
    copyrightInfoText.append("\n");
    copyrightInfoText.append("See <http://www.gnu.org/licenses/> for a copy of these licenses.\n");
    copyrightInfoText.append("\n");
}

VWTinterfaceDriver::~VWTinterfaceDriver()
{
    if (agaveConnector != NULL) delete agaveConnector;
    if (authWindow != NULL) delete authWindow;
    if (mainWindow != NULL) delete mainWindow;
    if (errorWindow != NULL) delete errorWindow;
}

void VWTinterfaceDriver::startup()
{
    authWindow = new AuthForm(this);
    mainWindow = new PanelWindow(this);

    QObject::connect(agaveConnector, SIGNAL(sendAuthResult(RequestState)), authWindow, SLOT(getAuthReply(RequestState)));
    //QObject::connect(agaveConnector, SIGNAL(sendFileData(QJsonValue)), fileWindow, SLOT(refreshFileInfo(QJsonValue)));
    QObject::connect(agaveConnector, SIGNAL(stateChanged(AgaveState,AgaveState)), this, SLOT(stateMonitor(AgaveState,AgaveState)));

    authWindow->show();
}

void VWTinterfaceDriver::hideWindowsForError()
{
    if (authWindow != NULL) authWindow->hide();
    if (mainWindow != NULL) mainWindow->hide();
}

void VWTinterfaceDriver::displayCopyInfo()
{
    QDialog copyInfoScreen;
    QVBoxLayout copyInfoLayout;

    QLabel copyInfoTextLabel(copyrightInfoText);
    QPushButton closeCopyWindow("Close Window");

    copyInfoLayout.addWidget(&copyInfoTextLabel);
    copyInfoLayout.addWidget(&closeCopyWindow);

    copyInfoScreen.setLayout(&copyInfoLayout);
    QObject::connect(&closeCopyWindow, SIGNAL(clicked(bool)), &copyInfoScreen, SLOT(accept()));

    copyInfoScreen.exec();
}

AgaveHandler * VWTinterfaceDriver::getAgaveConnection()
{
    return agaveConnector;
}

void VWTinterfaceDriver::stateMonitor(AgaveState oldState, AgaveState newState)
{
    if ((oldState == AgaveState::GETTING_AUTH) && (newState == AgaveState::LOGGED_IN))
    {
        closeAuthScreen();
    }
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
        authWindow->hide();
        authWindow->deleteLater();
        authWindow = NULL;
    }
}
