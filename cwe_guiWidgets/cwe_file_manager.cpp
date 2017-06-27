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

#include "cwe_file_manager.h"
#include "ui_cwe_file_manager.h"

#include <QFileDialog>

CWE_file_manager::CWE_file_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_file_manager)
{
    ui->setupUi(this);

    // Creates our new model and populate
    localFileModel = new QFileSystemModel(this);

    // Set filter
    localFileModel->setFilter(QDir::AllDirs | QDir::Files);
    ui->localListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Attach the model to the view
    ui->localListView->setModel(localFileModel);

    // QFileSystemModel requires root path
    localFileModel->setRootPath(QDir::homePath());
    ui->localListView->setRootIndex(localFileModel->setRootPath(QDir::homePath()));
}

CWE_file_manager::~CWE_file_manager()
{
    delete ui;
}

void CWE_file_manager::on_pb_upload_clicked()
{
    /* upload selected local files */
}

void CWE_file_manager::on_pb_download_clicked()
{
    /* download selected remote files */
}

void CWE_file_manager::on_localListView_doubleClicked(const QModelIndex &index)
{
    QDir mDir = localFileModel->filePath(index);
    if (localFileModel->isDir(index))
    {
       QString mPath = localFileModel->fileInfo(index).absoluteFilePath();
       ui->localListView->setRootIndex(localFileModel->setRootPath(mPath));
    }
}
