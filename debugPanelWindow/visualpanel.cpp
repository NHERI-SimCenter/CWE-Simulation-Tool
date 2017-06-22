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

#include "visualpanel.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "remoteFileOps/remotefiletree.h"
#include "remoteFileOps/filetreenode.h"
#include "visualUtils/cfdglcanvas.h"
#include "visualUtils/decompresswrapper.h"

VisualPanel::VisualPanel(RemoteDataInterface * newDataHandle, RemoteFileTree * newReader,
                         QStringList frameNames, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList(frameNames);

    myTreeReader = newReader;
    dataConnection = newDataHandle;
}

void VisualPanel::setupOwnFrame()
{
    QVBoxLayout * vLayout = new QVBoxLayout;

    myCanvas = new CFDglCanvas();

    vLayout->addWidget(myCanvas);
    getOwnedWidget()->setLayout(vLayout);
}

void VisualPanel::frameNowVisible()
{
    QObject::connect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
    myTreeReader->resendSelectedFile();
}

void VisualPanel::frameNowInvisible()
{
    QObject::disconnect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));

}

void VisualPanel::selectedFileChanged(FileMetaData * newSelection)
{
    conditionalPurge(&pointData);
    conditionalPurge(&faceData);
    conditionalPurge(&ownerData);
    myCanvas->setDisplayState(CFDDisplayState::TEST_BOX);

    if (newSelection->getFileType() == FileType::DIR)
    {
        FileTreeNode * currentNode = myTreeReader->getFileNodeFromPath(newSelection->getFullPath());

        FileTreeNode * constantFolder = currentNode->getChildNodeWithName("constant");
        if (constantFolder == NULL) return;
        FileTreeNode * polyMeshFolder = constantFolder->getChildNodeWithName("polyMesh");
        if (polyMeshFolder == NULL) return;
        FileTreeNode * pointsFile = polyMeshFolder->getChildNodeWithName("points");
        FileTreeNode * facesFile = polyMeshFolder->getChildNodeWithName("faces");
        FileTreeNode * ownerFile = polyMeshFolder->getChildNodeWithName("owner");
        if (pointsFile == NULL) pointsFile = polyMeshFolder->getChildNodeWithName("points.gz");
        if (facesFile == NULL) facesFile = polyMeshFolder->getChildNodeWithName("faces.gz");
        if (ownerFile == NULL) ownerFile = polyMeshFolder->getChildNodeWithName("owner.gz");

        if ((pointsFile == NULL) || (facesFile == NULL) || (ownerFile == NULL)) return;

        RemoteDataReply * aReply = dataConnection->downloadBuffer(pointsFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

        aReply = dataConnection->downloadBuffer(facesFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

        aReply = dataConnection->downloadBuffer(ownerFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));
    }
}

void VisualPanel::conditionalPurge(QByteArray ** theArray)
{
    if (*theArray == NULL) return;
    delete *theArray;
    *theArray = NULL;
}

void VisualPanel::gotNewRawFile(RequestState authReply, QByteArray * fileBuffer)
{
    if (authReply != RequestState::GOOD) return;

    RemoteDataReply * mySender = (RemoteDataReply *) QObject::sender();
    if (mySender == NULL) return;
    QString lookedForFile = mySender->getTaskParamList()->value("remoteName");
    if (lookedForFile.isEmpty()) return;

    QByteArray * realContents;

    if (lookedForFile.endsWith(".gz"))
    {
        lookedForFile.chop(3);
        DeCompressWrapper decompresser(fileBuffer);
        realContents = decompresser.getDecompressedFile();
    }
    else
    {
        realContents = new QByteArray(*fileBuffer);
    }
    if (lookedForFile.endsWith("points"))
    {
        conditionalPurge(&pointData);
        pointData = realContents;
    }
    if (lookedForFile.endsWith("faces"))
    {
        conditionalPurge(&faceData);
        faceData = realContents;
    }
    if (lookedForFile.endsWith("owner"))
    {
        conditionalPurge(&ownerData);
        ownerData = realContents;
    }
    if ((pointData != NULL) && (faceData != NULL) && (ownerData != NULL))
    {
        if(myCanvas->loadMeshData(pointData, faceData, ownerData))
        {
            myCanvas->setDisplayState(CFDDisplayState::MESH);
        }
    }
}
