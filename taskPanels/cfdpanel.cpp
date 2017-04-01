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

#include "cfdpanel.h"

#include "../vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../programWindows/filetreemodelreader.h"

CFDpanel::CFDpanel(RemoteDataInterface * newDataHandle, FileTreeModelReader * newReader, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList({"Run/Setup Simulation", ". . . Using OpenFOAM"});

    myTreeReader = newReader;
    dataConnection = newDataHandle;
    this->setFileTreeVisibleSetting(true);
}

void CFDpanel::setupOwnFrame()
{
    QVBoxLayout * vLayout = new QVBoxLayout;

    QLabel * headLabel = new QLabel("File Details:");
    vLayout->addWidget(headLabel);
    contentLabel = new QLabel("No File Selected.");
    vLayout->addWidget(contentLabel);

    startButton = new QPushButton("Begin CFD");
    QObject::connect(startButton,SIGNAL(clicked(bool)),this,SLOT(cfdSelected()));
    vLayout->addWidget(startButton);
    startButton->setVisible(false);

    getOwnedWidget()->setLayout(vLayout);
}

void CFDpanel::frameNowVisible()
{
    QObject::connect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
    myTreeReader->resendSelectedFile();
}

void CFDpanel::frameNowInvisible()
{
    QObject::disconnect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
}

void CFDpanel::selectedFileChanged(FileMetaData * newSelection)
{
    startButton->setVisible(false);

    if ((newSelection->getFileType() == FileType::EMPTY_FOLDER) ||
        (newSelection->getFileType() == FileType::INVALID) ||
        (newSelection->getFileType() == FileType::UNLOADED))
    {
        contentLabel->setText("No File Selected.");
        return;
    }

    contentLabel->setText(newSelection->getFileName().toLatin1());

    if (newSelection->getFileType() == FileType::DIR)
    {
        startButton->setVisible(true);
    }
}

void CFDpanel::cfdSelected()
{
    qDebug("Beginning CFD task");
    QMultiMap<QString, QString> oneInput;
    oneInput.insert("solver","pisoFoam");
    FileMetaData fileData = myTreeReader->getCurrentSelectedFile();
    if (fileData.getFileType() != FileType::DIR)
    {
        //TODO: give reasonable error
        return;
    }
    RemoteDataReply * compressTask = dataConnection->runRemoteJob("openfoam",oneInput,fileData.getFullPath());
    if (compressTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(compressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(finishedCFDinvoke(RequestState,QJsonDocument*)));
}

void CFDpanel::finishedCFDinvoke(RequestState finalState, QJsonDocument *)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}
