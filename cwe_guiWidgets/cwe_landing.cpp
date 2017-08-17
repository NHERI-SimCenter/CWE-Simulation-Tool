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

CWE_landing::CWE_landing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_landing)
{
    ui->setupUi(this);

    QTableView * m_listView = ui->tableView_jobs;

    MyTableModel *model;

    // Create model
    model = new MyTableModel(this);

    // Make data
    QStringList List;
    List << "name given" << "state" << "time created" << "ID" << "application";

    // Populate our model
    model->addStringList(List);

    // Glue model and view together
    ui->tableView_jobs->setModel(model);

    /*
    // Get the position
    int row = model->rowCount();

    // Enable add one or more rows
    model->insertRows(row,1);

    // Get the row for Edit mode
    QModelIndex index = model->index(row);

    // Enable item selection and put it edit mode
    ui->listView_jobs->setCurrentIndex(index);
    ui->listView_jobs->edit(index);
    */


    /* set some dummy contents */
    QString theState;
    QString theTimeCreated;
    QString theID;
    QString theApplication;

    uint state = CWE_STATE_NONE;

    if (state & CWE_STATE_NEW)     {theState = "new";}
    if (state & CWE_STATE_RUNNING) {theState = "running";}
    if (state & CWE_STATE_RESULTS) {theState = "done";}
    if (state & CWE_STATE_NONE)    {theState = "none";}
    if (state & CWE_STATE_CLEAR)   {theState = "clear";}

    theTimeCreated = QTime::currentTime().toString();
}

CWE_landing::~CWE_landing()
{
    delete ui;
}
