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

void ResultProcureBase::initializeWithNeededFiles(FileNodeRef baseFolder, QMap<QString, QString> neededFiles)
{
    if (!myFileNames.empty() || initLoadDone)
    {
        cwe_globals::displayPopup("Internal Error: Attempt made to double-initialize result display.");
        return;
    }

    if ((!baseFolder.fileNodeExtant()) || neededFiles.isEmpty())
    {
        initialFailure();
        return;
    }

    myBaseFolder = baseFolder;
    myFileNames = neededFiles;

    FileNodeRef nil;

    for (QString fileID : myFileNames.keys())
    {
        myFileNodes[fileID] = nil;
    }

    fileChanged(nil, FileSystemChange::FILE_MODIFY);
}

QMap<QString, FileNodeRef> ResultProcureBase::getFileNodes()
{
    return myFileNodes;
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

    for (QString fileID : myFileNodes.keys())
    {
        FileNodeRef theFile = myFileNodes.value(fileID);
        if (!theFile.fileNodeExtant())
        {
            cwe_globals::displayFatalPopup("Internal Error: result file not loaded after load");
            return;
        }
        QByteArray * rawBuffer = NULL;

        if (theFile.getFileName().endsWith(".gz"))
        {
            QByteArray compressedBuffer = theFile.getFileBuffer();
            DeCompressWrapper realPoints(&compressedBuffer);
            rawBuffer = realPoints.getDecompressedFile();
        }
        else
        {
            rawBuffer = new QByteArray(theFile.getFileBuffer());
        }

        if (rawBuffer == NULL)
        {
            cwe_globals::displayFatalPopup("Internal Error: result buffer not loaded after load");
            return;
        }
        myBufferList[fileID] = rawBuffer;
    }
}

void ResultProcureBase::fileChanged(FileNodeRef changedFile, FileSystemChange theChange)
{
    if (changedFile.isNil())
    {
        if (initLoadDone) return;
        if (checkForAndSeekFiles())
        {
            initLoadDone = true;
            allFilesLoaded();
        }
    }

    QString idChanged = getIDfromNode(changedFile);
    if (idChanged.isEmpty()) return;

    if (theChange == FileSystemChange::FILE_DELETE)
    {
        FileNodeRef nil;
        myFileNodes[idChanged] = nil;
    }

    if (initLoadDone)
    {
        underlyingDataChanged(idChanged);
        return;
    }

    if (checkForAndSeekFiles())
    {
        initLoadDone = true;
        allFilesLoaded();
    }
}

bool ResultProcureBase::checkForAndSeekFiles()
{
    //Return true if all files found
    //Invoke initial failure if files not extant
    //else return false

    for (QString fileID: myFileNames.keys())
    {
        QString fileName = myFileNames.value(fileID);
        FileNodeRef fileNode = myFileNodes.value(fileID);

        if (fileNode.isNil())
        {
            FileNodeRef targetFileNode;

            if (fileName.startsWith("[final]"))
            {
                if (!myBaseFolder.folderContentsLoaded())
                {
                    myBaseFolder.enactFolderRefresh();
                    continue;
                }

                fileName.remove(0,7);
                FileNodeRef lastResult = getFinalResultFolder();

                if (lastResult.isNil())
                {
                    QObject::disconnect(this);
                    initialFailure();
                    return false;
                }

                targetFileNode = cwe_globals::get_file_handle()->speculateFileWithName(lastResult, fileName, false);
            }
            else if (fileName.endsWith(".gz"))
            {
                targetFileNode = cwe_globals::get_file_handle()->speculateFileWithName(myBaseFolder, fileName, false);

                if (targetFileNode.isNil())
                {
                    fileName.chop(3);
                    targetFileNode = cwe_globals::get_file_handle()->speculateFileWithName(myBaseFolder, fileName, false);
                }
            }
            else
            {
                targetFileNode = cwe_globals::get_file_handle()->speculateFileWithName(myBaseFolder, fileName, false);
            }

            if (targetFileNode.isNil())
            {
                QObject::disconnect(this);
                initialFailure();
                return false;
            }

            myFileNodes[fileID] = targetFileNode;
            QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileSystemChange(FileTreeNode*,FileSystemChange)),
                             this, SLOT(fileChanged(FileTreeNode*, FileSystemChange)),
                             Qt::QueuedConnection);
            fileNode = targetFileNode;
        }

        if (!fileNode.fileBufferLoaded())
        {
            cwe_globals::get_file_handle()->sendDownloadBuffReq(fileNode);
        }
    }

    for (FileNodeRef aNode : myFileNodes)
    {
        if (aNode.isNil()) return false;
        if (!aNode.fileBufferLoaded()) return false;
    }

    return true;
}

FileNodeRef ResultProcureBase::getFinalResultFolder()
{
    double biggestNum = -1.0;
    FileNodeRef targetChild;

    for (FileNodeRef childNode : myBaseFolder.getChildList())
    {
        if (childNode.getFileType() != FileType::DIR) continue;

        QString childName = childNode.getFileName();
        if (childName == "0") continue;

        bool isNum = false;
        double childVal = childName.toDouble(&isNum);
        if (!isNum) continue;
        {
            if (biggestNum < childVal)
            {
                biggestNum = childVal;
                targetChild = childNode;
            }
        }
    }

    return targetChild;
}

QString ResultProcureBase::getIDfromNode(FileNodeRef fileNode)
{
    QString ret;
    if (fileNode.isNil()) return ret;

    for (QString anID : myFileNodes.keys())
    {
        FileNodeRef aNode = myFileNodes[anID];

        if (aNode.getFullPath() == fileNode.getFullPath()) return anID;
    }
    return ret;
}
