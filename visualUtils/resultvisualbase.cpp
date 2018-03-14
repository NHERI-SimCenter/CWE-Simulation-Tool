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

#include "resultvisualbase.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

ResultVisualBase::ResultVisualBase(QObject *parent) : QObject(parent)
{

}

ResultVisualBase::~ResultVisualBase()
{

}

void ResultVisualBase::initializeWithNeededFiles(FileTreeNode * baseFolder, QList<QString> neededFiles)
{


    fileRecordsChanged();
}

void ResultVisualBase::baseFolderRemoved()
{
    QObject::disconnect(this); //TODO: Make specific to given signal
    dataLostError();
}

void ResultVisualBase::fileRecordsChanged()
{
    bool fileMissing = false;
    QMap<QString, QByteArray *> rawFileData;

    for (auto itr = myFileList.cbegin(); itr != myFileList.cend(); itr++)
    {
        QString filePath = *itr;

        FileTreeNode * theDataNode = myBaseFolder->getNodeReletiveToNodeWithName(filePath);

        if (theDataNode == NULL)
        {
            fileMissing = true;
        }
        else if (theDataNode->getFileBuffer() == NULL)
        {
            fileMissing = true;

        }
        else
        {
            rawFileData.insert(filePath, theDataNode->getFileBuffer());
        }
    }

    if (!fileMissing)
    {
        QObject::disconnect(this); //TODO: Make specific to given signal
        allFilesLoaded(rawFileData);
        return;
    }

}
