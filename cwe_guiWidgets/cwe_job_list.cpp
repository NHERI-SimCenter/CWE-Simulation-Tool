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
// Peter Sempolinski, University of Notre Dame

#include "cwe_job_list.h"
#include "ui_cwe_job_list.h"

#include "cwe_interfacedriver.h"
#include "cwe_globals.h"
#include "remoteJobs/joboperator.h"
#include "remotejobdata.h"
#include "remotedatainterface.h"

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

void CWE_job_list::linkMainWindow(CWE_MainWindow *theMainWin)
{
    //TODO: Clean up init
    CWE_Super::linkMainWindow(theMainWin);
    if (!cwe_globals::get_CWE_Driver()->inOfflineMode())
    {
        ui->tableView_jobs->setOperator(cwe_globals::get_job_handle());
        QObject::connect(ui->tableView_jobs, SIGNAL(customContextMenuRequested(QPoint)),
                         this, SLOT(customJobMenu(QPoint)));
        QObject::connect(cwe_globals::get_job_handle(), SIGNAL(jobOpDone(RequestState, QString)),
                         this, SLOT(jobOpDone(RequestState, QString)), Qt::QueuedConnection);
    }
}

void CWE_job_list::customJobMenu(const QPoint &pos)
{
    QModelIndex targetIndex = ui->tableView_jobs->indexAt(pos);
    QMenu jobMenu;

    ui->tableView_jobs->jobEntryTouched(targetIndex);
    RemoteJobData theJob = ui->tableView_jobs->getSelectedJob();

    //If we did not click anything, we should return
    if (!theJob.isValidEntry()) return;

    if (cwe_globals::get_job_handle()->currentlyPerformingJobOperation())
    {
        jobMenu.addAction("Performing Job Operation . . .");
        jobMenu.exec(QCursor::pos());
        return;
    }

    if (cwe_globals::get_job_handle()->currentlyPerformingJobOperation())
    {
        jobMenu.addAction("Refreshing File List . . .");
        jobMenu.exec(QCursor::pos());
        return;
    }

    targetJob = theJob;

    jobMenu.addAction("Delete Job Entry",this, SLOT(deleteJobItem()));
    jobMenu.exec(QCursor::pos());
}

void CWE_job_list::deleteJobItem()
{
    if (cwe_globals::get_job_handle()->currentlyPerformingJobOperation()) return;
    cwe_globals::get_job_handle()->deleteJobDataEntry(&targetJob);
    expectingOp = cwe_globals::get_job_handle()->currentlyPerformingJobOperation();
}

void CWE_job_list::jobOpDone(RequestState opState, QString err_msg)
{
    if (!expectingOp) return;
    expectingOp = false;

    if (opState != RequestState::GOOD)
    {
        cwe_globals::displayPopup(err_msg,"Job Operation");
    }
    else
    {
        cwe_globals::displayPopup("Job Operation Complete","Job Operation");
    }
}
