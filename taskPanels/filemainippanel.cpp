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

#include "filemainippanel.h"

#include "../vwtinterfacedriver.h"
#include "../agaveInterfaces/filetreemodelreader.h"
#include "../agaveInterfaces/agavehandler.h"
#include "../agaveInterfaces/agavetaskreply.h"

FileMainipPanel::FileMainipPanel(AgaveHandler * newAgaveHandle, FileTreeModelReader * newReader, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList({"Remote File Management", "Browse Remote Files"});

    myTreeReader = newReader;
    agaveConnection = newAgaveHandle;
    this->setFileTreeVisibleSetting(true);
}

void FileMainipPanel::setupOwnFrame()
{
    QVBoxLayout * vLayout = new QVBoxLayout;

    QLabel * headLabel = new QLabel("File Details:");
    vLayout->addWidget(headLabel);
    contentLabel = new QLabel("No File Selected.");
    vLayout->addWidget(contentLabel);

    getOwnedWidget()->setLayout(vLayout);
}

void FileMainipPanel::frameNowVisible()
{
    myTreeReader->setRightClickMenuEnabled(true);
    selectedFileChanged(myTreeReader->getCurrentSelectedFile());
    QObject::connect(myTreeReader, SIGNAL(newFileSelected(QModelIndex)), this, SLOT(selectedFileChanged(QModelIndex)));
}

void FileMainipPanel::frameNowInvisible()
{
    myTreeReader->setRightClickMenuEnabled(false);
    QObject::disconnect(myTreeReader, SIGNAL(newFileSelected(QModelIndex)), this, SLOT(selectedFileChanged(QModelIndex)));
}

void FileMainipPanel::selectedFileChanged(QModelIndex newSelection)
{
    QMap<QString, QString> fileDataList = myTreeReader->getDataForEntry(newSelection);
    if (fileDataList.isEmpty())
    {
        contentLabel->setText("No File Selected.");
        return;
    }
    //TODO: Need to make a more user friendly view
    QString blindDump;
    QList<QString> keyList = fileDataList.keys();
    for (auto itr = keyList.cbegin(); itr != keyList.cend(); ++itr)
    {
        blindDump.append(*itr);
        blindDump.append(": ");
        blindDump.append(fileDataList.value(*itr));
        blindDump.append("\n");
    }
    contentLabel->setText(blindDump);
}
