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
// Peter Mackenzie-Helnwein, UW Seattle

#include "cwe_job_list.h"
#include "ui_cwe_job_list.h"

#include "cwe_interfacedriver.h"
#include "cwe_globals.h"
#include "remoteFiles/remotefiletree.h"

CWE_job_list::CWE_job_list(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_job_list)
{
    ui->setupUi(this);
}

CWE_job_list::~CWE_job_list()
{
    delete ui;
}

void CWE_job_list::customJobMenu(const QPoint &)
{
    //TODO
    /*
    QModelIndex targetIndex = ui->tableView_jobs->indexAt(pos);
    ui->tableView_jobs->

    QMenu jobMenu;

    targetNode = ui->remoteTreeView->getSelectedFile();

    //If we did not click anything, we should return
    if (targetNode.isNil()) return;
    if (targetNode.isRootNode()) return;

    if (targetNode.getFileType() == FileType::INVALID) return;

    fileMenu.addAction("Copy To . . .",this, SLOT(copyMenuItem()));
    fileMenu.addAction("Move To . . .",this, SLOT(moveMenuItem()));
    //We don't let the user delete the username folder

    if (targetNode.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("Download Buffer (DEBUG)",this, SLOT(downloadBufferItem()));
    }

    if ((targetNode.getFileType() == FileType::DIR) || (targetNode.getFileType() == FileType::FILE))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Refresh Data",this, SLOT(refreshMenuItem()));
        fileMenu.addSeparator();
    }

    fileMenu.exec(QCursor::pos());
    */
}
