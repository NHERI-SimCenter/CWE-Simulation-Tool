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

#include "remotedatainterface.h"

FileMetaData::FileMetaData()
{
    //Note: defaults are handled in the class header
}

FileMetaData::FileMetaData(FileMetaData * copyFrom)
{
    this->fullContainingPath = copyFrom->fullContainingPath;
    this->fileName = copyFrom->fileName;
    this->fileSize = copyFrom->fileSize;
    this->myType = copyFrom->myType;
}

FileMetaData FileMetaData::operator=(const FileMetaData & toCopy)
{
    FileMetaData ret(&toCopy);
    return ret;
}

void FileMetaData::setFullFilePath(QString fullPath)
{
    QStringList pathParts;
    char divChar;
    if (fullPath.contains('/'))
    {
        pathParts = fullPath.split('/');
        divChar = '/';
    }
    else
    {
        pathParts = fullPath.split('\\');
        divChar = '\\';
    }
    fileName = pathParts.at(pathParts.size() - 1);
    pathParts.removeLast();
    fullContainingPath = divChar;
    for (auto itr = pathParts.cbegin(); itr != pathParts.cend(); itr++)
    {
        fullContainingPath.append(*itr);
        fullContainingPath.append(divChar);
    }
}

void FileMetaData::setSize(int newSize)
{
    fileSize = newSize;
}

void FileMetaData::setType(FileType newType)
{
    myType = newType;
}

QString FileMetaData::getFullPath()
{
    QString ret = fullContainingPath;
    ret.append(fileName);
    return ret;
}

QString FileMetaData::getFileName()
{
    return fileName;
}

QString FileMetaData::getContainingPath()
{
    return fullContainingPath;
}

int FileMetaData::getSize()
{
    return fileSize;
}

FileType FileMetaData::getFileType()
{
    return myType;
}
