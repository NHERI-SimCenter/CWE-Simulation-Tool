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

#include "vwtinterfacedriver.h"

CWE_job_list::CWE_job_list(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_job_list)
{
    ui->setupUi(this);

    // Create model
    model = new QStandardItemModel(this);

    QStringList HeaderList;
    HeaderList << "name given" << "state" << "time created" << "ID" << "application";
    model->setHorizontalHeaderLabels(HeaderList);

    // Glue model and view together
    ui->tableView_jobs->setModel(model);
    ui->tableView_jobs->verticalHeader()->setVisible(false);

    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Stretch);
}

CWE_job_list::~CWE_job_list()
{
    delete ui;
}

void CWE_job_list::linkDriver(VWTinterfaceDriver * theDriver)
{
    CWE_Super::linkDriver(theDriver);
    if (!theDriver->inOfflineMode())
    {
        ui->tableView_jobs->setJobHandle(theDriver->getJobHandler());
    }
}
