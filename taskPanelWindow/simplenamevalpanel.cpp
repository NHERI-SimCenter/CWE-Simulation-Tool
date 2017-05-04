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

#include "simplenamevalpanel.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "fileWindow/remotefilewindow.h"

SimpleNameValPanel::SimpleNameValPanel(RemoteDataInterface *newDataHandle, RemoteFileWindow *newReader,
                                       QStringList frameNames, QStringList indirectParams,
                                       QStringList directParams, QString composedParam,
                                       QString newAppName, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList(frameNames);
    composedParamName = composedParam;
    indirectParamList = indirectParams;
    directParamList = directParams;

    myTreeReader = newReader;
    dataConnection = newDataHandle;
    appName = newAppName;
}

void SimpleNameValPanel::setupOwnFrame()
{
    QVBoxLayout * vLayout = new QVBoxLayout;

    for (auto itr = indirectParamList.cbegin(); itr != indirectParamList.cend(); itr++)
    {
        QString itemName = (*itr);
        QHBoxLayout * nameValPair = new QHBoxLayout;

        QLabel * headLabel = new QLabel(itemName);
        nameValPair->addWidget(headLabel);

        QLineEdit * dataEntry = new QLineEdit();
        indirectParamBoxes.append(dataEntry);
        nameValPair->addWidget(dataEntry);
        vLayout->addLayout(nameValPair);
    }

    for (auto itr = directParamList.cbegin(); itr != directParamList.cend(); itr++)
    {
        QString itemName = (*itr);
        QHBoxLayout * nameValPair = new QHBoxLayout;

        QLabel * headLabel = new QLabel(itemName);
        nameValPair->addWidget(headLabel);

        QLineEdit * dataEntry = new QLineEdit();
        dataEntry->setObjectName(itemName);
        directParamBoxes.append(dataEntry);
        nameValPair->addWidget(dataEntry);
        vLayout->addLayout(nameValPair);
    }

    startButton = new QPushButton("Begin App");
    QObject::connect(startButton,SIGNAL(clicked(bool)),this,SLOT(appInvoked()));
    vLayout->addWidget(startButton);
    startButton->setVisible(false);

    getOwnedWidget()->setLayout(vLayout);
}

void SimpleNameValPanel::frameNowVisible()
{
    QObject::connect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
    myTreeReader->resendSelectedFile();
}

void SimpleNameValPanel::frameNowInvisible()
{
    QObject::disconnect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
}

void SimpleNameValPanel::selectedFileChanged(FileMetaData * newSelection)
{
    if (newSelection->getFileType() == FileType::DIR)
    {
        startButton->setVisible(true);
    }
}

void SimpleNameValPanel::appInvoked()
{
    qDebug("Beginning CFD task");
    FileMetaData fileData = myTreeReader->getCurrentSelectedFile();

    QMultiMap<QString, QString> inputList;
    QString fullIndirectInput;
    for (auto itr = indirectParamBoxes.cbegin(); itr != indirectParamBoxes.cend(); itr++)
    {
        if (!((*itr)->text().isEmpty()))
        {
            fullIndirectInput.append((*itr)->objectName());
            fullIndirectInput.append(" ");
            fullIndirectInput.append((*itr)->text());
            fullIndirectInput.append(" ");
        }
    }
    inputList.insert(composedParamName, fullIndirectInput);

    for (auto itr = directParamBoxes.cbegin(); itr != directParamBoxes.cend(); itr++)
    {
        inputList.insert((*itr)->objectName(), (*itr)->text());
    }

    RemoteDataReply * remoteTask = dataConnection->runRemoteJob(appName,inputList,fileData.getFullPath());
    if (remoteTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(remoteTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(finishedAppInvoke(RequestState,QJsonDocument*)));
}

void SimpleNameValPanel::finishedAppInvoke(RequestState finalState, QJsonDocument * rawData)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}
