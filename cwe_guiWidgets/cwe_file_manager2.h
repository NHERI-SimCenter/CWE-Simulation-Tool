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

#ifndef CWE_FILE_MANAGER2_H
#define CWE_FILE_MANAGER2_H

#include "cwe_super.h"

#include <QFileSystemModel>
#include <QMenu>

class FileTreeNode;

namespace Ui {
class CWE_file_manager2;
}

class CWE_file_manager2 : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_file_manager2(QWidget *parent = 0);
    ~CWE_file_manager2();

    virtual void linkDriver(VWTinterfaceDriver * theDriver);

private slots:
    void on_pb_upload_clicked();
    void on_pb_download_clicked();

    void customFileMenu(const QPoint &pos);
    void copyMenuItem();
    void moveMenuItem();
    void renameMenuItem();
    void deleteMenuItem();
    void createFolderMenuItem();

    void compressMenuItem();
    void decompressMenuItem();
    void refreshMenuItem();

    void downloadBufferItem();

    void remoteOpDone();

    void on_localButton_newFolder_clicked();
    void on_localButton_deleteFolder_clicked();
    void on_localButton_deleteFile_clicked();

    void on_remoteButton_newFolder_clicked();
    void on_remoteButton_deleteFolder_clicked();
    void on_remoteButton_deleteFile_clicked();

private:
    Ui::CWE_file_manager2 *ui;
    QFileSystemModel *localFileModel;

    FileTreeNode * targetNode = NULL;
};

#endif // CWE_FILE_MANAGER2_H
