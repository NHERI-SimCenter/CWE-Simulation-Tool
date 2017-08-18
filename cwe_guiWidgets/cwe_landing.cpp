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

#include "cwe_landing.h"
#include "ui_cwe_landing.h"

#include "cwe_defines.h"
#include <QTime>
#include "mytablemodel.h"

#include "../AgaveExplorer/remoteFileOps/remotejoblister.h"
#include "../AgaveExplorer/remoteFileOps/joboperator.h"

CWE_landing::CWE_landing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_landing)
{
    ui->setupUi(this);

    // Create model
    model = new MyTableModel(this);

    QStringList HeaderList;
    HeaderList << "name given" << "state" << "time created" << "ID" << "application";
    model->setHeaders(HeaderList);

    // Glue model and view together
    ui->tableView_jobs->setModel(model);
    ui->tableView_jobs->verticalHeader()->setVisible(false);

    ui->tableView_jobs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->tableView_jobs->horizontalHeader()->setSizePolicy();

    /* set some dummy contents */
    this->addDummyDataRow();
}

CWE_landing::~CWE_landing()
{
    delete ui;
}

void CWE_landing::linkJobHandle(JobOperator * theOperator)
{
    ui->tableView_jobs->setJobHandle(theOperator);
}

void CWE_landing::addDataRow(QString name, uint state, QString time, QString id, QString app)
{
    QString theState;

    if (state & CWE_STATE_NEW)          {theState = "new";}
    else if (state & CWE_STATE_RUNNING) {theState = "running";}
    else if (state & CWE_STATE_RESULTS) {theState = "done";}
    else if (state & CWE_STATE_NONE)    {theState = "none";}
    else if (state & CWE_STATE_CLEAR)   {theState = "clear";}
    else {theState = "undefined";}

    // Make data
    QStringList List;
    List << name << theState << time << id << app;

    // Populate our model
    model->addStringList(List);
}

void CWE_landing::addDummyDataRow(void)
{
    /* set some dummy contents */
    this->addDataRow("some URL", CWE_STATE_CLEAR, QTime::currentTime().toString(), "007", "SimCenter super app");
}
