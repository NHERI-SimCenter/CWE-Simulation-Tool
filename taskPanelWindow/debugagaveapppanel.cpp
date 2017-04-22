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

#include "debugagaveapppanel.h"

#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "fileWindow/remotefilewindow.h"

DebugAgaveAppPanel::DebugAgaveAppPanel(RemoteDataInterface * newDataHandle, RemoteFileWindow * newReader, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList({"Debug", "Test Agave App"});

    myTreeReader = newReader;
    dataConnection = newDataHandle;

    agaveAppList.appendRow(new QStandardItem("FileEcho"));
    inputLists.insert("FileEcho", {"NewFile", "EchoText"});
    agaveAppList.appendRow(new QStandardItem("PythonTest"));
    inputLists.insert("PythonTest", {"NewFile"});
    agaveAppList.appendRow(new QStandardItem("SectionMesh"));
    inputLists.insert("SectionMesh", {"directory", "SlicePlane", "MeshParams"});
}

void DebugAgaveAppPanel::setupOwnFrame()
{
    vLayout = new QVBoxLayout;

    agaveOptionList = new QListView();
    agaveOptionList->setModel(&agaveAppList);
    agaveOptionList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QObject::connect(agaveOptionList,SIGNAL(clicked(QModelIndex)), this, SLOT(placeInputPairs(QModelIndex)));
    vLayout->addWidget(agaveOptionList);

    startButton = new QPushButton("Start Agave App");
    QObject::connect(startButton,SIGNAL(clicked(bool)),this,SLOT(commandInvoked()));
    vLayout->addWidget(startButton);

    getOwnedWidget()->setLayout(vLayout);
}

void DebugAgaveAppPanel::placeInputPairs(QModelIndex newSelected)
{
    if (buttonArea != NULL)
    {
        QLayoutItem * toDelete;
        toDelete = buttonArea->takeAt(0);
        while (toDelete != NULL)
        {
            delete toDelete->widget();
            delete toDelete;
            toDelete = buttonArea->takeAt(0);
        }
        delete buttonArea;
        buttonArea = NULL;
    }

    QString selectedApp = agaveAppList.itemFromIndex(newSelected)->text();
    if (selectedApp.isEmpty())
    {
        return;
    }

    QStringList inputList = inputLists.value(selectedApp);

    if (inputList.size() <= 0)
    {
        return;
    }

    buttonArea = new QGridLayout();
    int rowNum = 0;

    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QLabel * tmpLabel = new QLabel(*itr);
        QLineEdit * tmpInput = new QLineEdit();
        tmpInput->setObjectName(*itr);

        buttonArea->addWidget(tmpLabel,rowNum,0);
        buttonArea->addWidget(tmpInput,rowNum,1);
        rowNum++;
    }

    vLayout->addLayout(buttonArea);
}

void DebugAgaveAppPanel::commandInvoked()
{
    if (waitingOnCommand)
    {
        return;
    }

    qDebug("Agave Command Test Invoked");
    QString selectedApp = agaveAppList.itemFromIndex(agaveOptionList->currentIndex())->text();
    qDebug("Selected App: %s", qPrintable(selectedApp));

    QString workingDir = myTreeReader->getCurrentSelectedFile().getFullPath();
    qDebug("Working Dir: %s", qPrintable(workingDir));

    QStringList inputList = inputLists.value(selectedApp);
    QMultiMap<QString, QString> allInputs;

    qDebug("Input List:");
    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QLineEdit * theInput = this->getOwnedWidget()->findChild<QLineEdit *>(*itr);
        if (theInput != NULL)
        {
            allInputs.insert((*itr),theInput->text());
            qDebug("%s : %s", qPrintable(*itr), qPrintable(theInput->text()));
        }
    }

    RemoteDataReply * theTask = dataConnection->runRemoteJob(selectedApp,allInputs,workingDir);
    if (theTask == NULL)
    {
        qDebug("Unable to invoke task");
        //TODO: give reasonable error
        return;
    }
    waitingOnCommand = true;
    expectedCommand = selectedApp;
    QObject::connect(theTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(commandReply(RequestState,QJsonDocument*)));
}

void DebugAgaveAppPanel::commandReply(RequestState finalState, QJsonDocument *)
{
    waitingOnCommand = false;
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}
