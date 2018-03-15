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

#include "resultprocurebase.h"
#include "cwe_globals.h"
#include "decompresswrapper.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"
#include "../AgaveExplorer/remoteFileOps/fileoperator.h"

#include "../AgaveClientInterface/filemetadata.h"

ResultProcureBase::ResultProcureBase(QWidget *parent) : QWidget(parent) {}

ResultProcureBase::~ResultProcureBase()
{
    for (auto itr = myBufferList.cbegin(); itr != myBufferList.cend(); itr++)
    {
        delete (*itr);
    }
}

void ResultProcureBase::initializeWithNeededFiles(FileTreeNode * baseFolder, QMap<QString, QString> neededFiles)
{
    if (!myFileList.empty() || initLoadDone)
    {
        cwe_globals::displayPopup("Internal Error: Attempt made to double-initialize result display.");
        return;
    }

    if ((baseFolder == NULL) || neededFiles.isEmpty())
    {
        initialFailure();
        return;
    }

    QObject::connect(baseFolder, SIGNAL(destroyed(QObject*)),
                     this, SLOT(baseFolderRemoved()));

    for (QString fileID: neededFiles.keys())
    {
        QString fileName = neededFiles.value(fileID);

        FileTreeNode * targetFileNode = cwe_globals::get_file_handle()->speculateNodeWithName(baseFolder, fileName, false);

        if ((targetFileNode == NULL) && fileName.endsWith(".gz"))
        {
            fileName.chop(3);
            targetFileNode = cwe_globals::get_file_handle()->speculateNodeWithName(baseFolder, fileName, false);
        }

        if ((targetFileNode == NULL) || !targetFileNode->isFile())
        {
            myFileList.clear();
            QObject::disconnect(this);
            initialFailure();
            return;
        }
        myFileList.insert(fileID,targetFileNode);
        QObject::connect(targetFileNode, SIGNAL(fileDataChanged(FileTreeNode*)),
                         this, SLOT(fileChanged(FileTreeNode*)));
        QObject::connect(targetFileNode, SIGNAL(destroyed(QObject*)),
                         this, SLOT(fileRemoved()));
    }
    fileChanged(NULL);
}

QMap<QString, FileTreeNode *> ResultProcureBase::getFileList()
{
    return myFileList;
}

QMap<QString, QByteArray *> ResultProcureBase::getFileBuffers()
{
    if (!initLoadDone)
    {
        qDebug("ERROR: File buffer compute request before files retrieved.");
        QMap<QString, QByteArray *> empty;
        return empty;
    }

    if (myBufferList.isEmpty())
    {
        computeFileBuffers();
    }
    return myBufferList;
}

void ResultProcureBase::computeFileBuffers()
{
    myBufferList.clear();

    for (QString fileID : myFileList.keys())
    {
        FileTreeNode * theFile = myFileList.value(fileID);
        if (theFile == NULL)
        {
            cwe_globals::displayFatalPopup("Internal Error: result file not loaded after load");
            return;
        }
        QByteArray * rawBuffer = NULL;

        if (theFile->getFileData().getFileName().endsWith(".gz"))
        {
            DeCompressWrapper realPoints(theFile->getFileBuffer());
            rawBuffer = realPoints.getDecompressedFile();
        }
        else
        {
            rawBuffer = new QByteArray(*(theFile->getFileBuffer()));
        }

        if (rawBuffer == NULL)
        {
            cwe_globals::displayFatalPopup("Internal Error: result buffer not loaded after load");
            return;
        }
        myBufferList[fileID] = rawBuffer;
    }
}

void ResultProcureBase::fileChanged(FileTreeNode * changedFile)
{
    if (initLoadDone)
    {
        underlyingDataChanged(changedFile, true);
        return;
    }
    bool foundAll = true;
    for (FileTreeNode * aFile: myFileList.values())
    {
        NodeState theFileState = aFile->getNodeState();
        if ((theFileState == NodeState::FILE_SPECULATE_IDLE) ||
                (theFileState == NodeState::FILE_KNOWN))
        {
            cwe_globals::get_file_handle()->sendDownloadBuffReq(aFile);
        }
        if (theFileState != NodeState::FILE_BUFF_LOADED)
        {
            foundAll = false;
        }
    }
    if (foundAll)
    {
        initLoadDone = true;
        allFilesLoaded();
    }
}

void ResultProcureBase::fileRemoved()
{
    FileTreeNode * removedFile = qobject_cast<FileTreeNode *>(QObject::sender());
    if (removedFile == NULL)
    {
        cwe_globals::displayFatalPopup("Internal Error: signal to result screen is mismatched object");
        return;
    }

    if (initLoadDone)
    {
        underlyingDataChanged(removedFile, false);
        return;
    }

    QString fileToLose = removedFile->getFileData().getFullPath();
    if (fileToLose.endsWith(".gz"))
    {
        fileToLose.chop(3);
        FileTreeNode * newNode = cwe_globals::get_file_handle()->speculateNodeWithName(fileToLose, false);

        if (newNode == NULL)
        {
            QObject::disconnect(this);
            initialFailure();
            return;
        }

        for (QString fileID: myFileList.keys())
        {
            FileTreeNode * searchNode = myFileList.value(fileID);
            if (searchNode == removedFile)
            {
                myFileList[fileID] = newNode;
                fileChanged(newNode);
                return;
            }
        }
    }

    QObject::disconnect(this);
    initialFailure();
    return;
}

void ResultProcureBase::baseFolderRemoved()
{
    QObject::disconnect(this);
    cwe_globals::displayPopup("Underlying case for displayed result has been deleted.");
    this->deleteLater();
}
