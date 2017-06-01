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

#include "decompresswrapper.h"

DeCompressWrapper::DeCompressWrapper(QByteArray *ref)
{
    myRefArray = ref;
}

QByteArray * DeCompressWrapper::getDecompressedFile()
{
    QByteArray * myResultArray = NULL;

    if (myRefArray == NULL)
    {
        return NULL;
    }

    QTemporaryFile compressedFile;
    if (!compressedFile.open())
    {
        return NULL;
    }
    QByteArray qFileName = compressedFile.fileName().toLatin1();
    const char * realFileName = qFileName.data();
    compressedFile.write(*myRefArray);
    compressedFile.close();

    gzFile compressHandle = gzopen(realFileName, "rb");

    myResultArray = new QByteArray();

    int resultVal = 1;

    while (resultVal > 0)
    {
        int buffLen = 1024;
        char dataBuff[buffLen];
        resultVal = gzread(compressHandle, dataBuff ,buffLen);
        if (resultVal < 0)
        {
            delete myResultArray;
            myResultArray = NULL;
            return NULL;
        }
        myResultArray->append(dataBuff, resultVal);
    }
    gzclose(compressHandle);

    return myResultArray;
}

QByteArray * DeCompressWrapper::getConditionalCompressedFileContents(QString fileName)
{
    QFile uncompressedFile(fileName);
    if (uncompressedFile.exists())
    {
        QByteArray * ret = NULL;
        uncompressedFile.open(QIODevice::ReadOnly);
        *ret = uncompressedFile.readAll();
        uncompressedFile.close();
        return ret;
    }

    fileName = fileName.append(".gz");

    QFile compressedFile(fileName);
    if (!compressedFile.exists())
    {
        return NULL;
    }

    compressedFile.open(QIODevice::ReadOnly);
    QByteArray rawContent = compressedFile.readAll();
    compressedFile.close();

    DeCompressWrapper inflater(&rawContent);

    return inflater.getDecompressedFile();
}
