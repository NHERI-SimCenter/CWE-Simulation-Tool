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

#include "cwe_manage_simulation.h"
#include "ui_cwe_manage_simulation.h"
#include "cwe_defines.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "vwtinterfacedriver.h"

CWE_manage_simulation::CWE_manage_simulation(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_manage_simulation)
{
    ui->setupUi(this);

    QObject::connect(ui->treeView, SIGNAL(newFileSelected(FileTreeNode*)),
                             this, SLOT(newFileSelected(FileTreeNode*)));

    clearSelectView();

    ui->stageListView->setModel(&stageListModel);
}

CWE_manage_simulation::~CWE_manage_simulation()
{
    delete ui;
}

void CWE_manage_simulation::linkDriver(VWTinterfaceDriver * theDriver)
{
    CWE_Super::linkDriver(theDriver);
    if (!theDriver->inOfflineMode())
    {
        ui->treeView->setFileOperator(theDriver->getFileHandler());
    }
}

void CWE_manage_simulation::newFileSelected(FileTreeNode * newFile)
{
    if (tempCase != NULL)
    {
        tempCase->deleteLater();
    }
    if (newFile == NULL)
    {
        tempCase = NULL;
        ui->label_caseStatus->setCurrentCase(NULL);
        clearSelectView();
        return;
    }

    tempCase = new CFDcaseInstance(newFile, myDriver);
    ui->label_caseStatus->setCurrentCase(tempCase);
    QObject::connect(tempCase, SIGNAL(haveNewState(CaseState)), this, SLOT(provisionalCaseStateChange(CaseState)));
    provisionalCaseStateChange(tempCase->getCaseState());
}

void CWE_manage_simulation::on_pb_viewParameters_clicked()
{
    if (!verifyCaseAndSelect()) return;

    // switch main window to parameters tab
    myDriver->getMainWindow()->switchToParameterTab();
}

void CWE_manage_simulation::on_pb_viewResults_clicked()
{
    if (!verifyCaseAndSelect()) return;

    // switch main window to results tab
    myDriver->getMainWindow()->switchToResultsTab();
}

void CWE_manage_simulation::clearSelectView()
{
    ui->label_caseTypeTag->setVisible(false);
    ui->label_CaseTypeIcon->setVisible(false);
    ui->label_stageListTag->setVisible(false);
    ui->stageListView->setVisible(false);
}

void CWE_manage_simulation::showSelectView()
{
    ui->label_caseTypeTag->setVisible(true);
    ui->label_CaseTypeIcon->setVisible(true);
    ui->label_stageListTag->setVisible(true);
    ui->stageListView->setVisible(true);
}

void CWE_manage_simulation::provisionalCaseStateChange(CaseState newState)
{
    //TODO: From PRS: I was not feeling well when writing this.
    //My instinct says this code should be reviewed.

    if (tempCase == NULL)
    {
        clearSelectView();
        return;
    }

    if ((newState == CaseState::JOB_RUN) || (newState == CaseState::READY))
    {
        ui->pb_viewParameters->setEnabled(true);
        ui->pb_viewResults->setEnabled(true);
    }
    else
    {
        ui->pb_viewParameters->setEnabled(false);
        ui->pb_viewResults->setEnabled(false);
    }

    if ((newState == CaseState::DEFUNCT)
             || (newState == CaseState::ERROR)
             || (newState == CaseState::INVALID))
    {
        tempCase->deleteLater();
        tempCase = NULL;
        ui->label_caseStatus->setCurrentCase(NULL);
        ui->label_caseName->setText("N/A");
        clearSelectView();
        return;
    }
    else if (newState == CaseState::LOADING)
    {
        clearSelectView();
    }
    else
    {
        CFDanalysisType * theType = tempCase->getMyType();
        if (theType == NULL)
        {
            myDriver->fatalInterfaceError("Type/stage mismatch for case.");
            return;
        }
        ui->label_CaseTypeIcon->setPixmap(theType->getIcon()->pixmap(150,100));

        QMap<QString, StageState> stages = tempCase->getStageStates();
        stageListModel.clear();

        for (auto itr = stages.cbegin(); itr != stages.cend(); itr++)
        {
            QString aLine = tempCase->getMyType()->translateStageId(itr.key());
            aLine = aLine.append(" - ");
            aLine = aLine.append(getStateText(*itr));
            stageListModel.appendRow(new QStandardItem(aLine));
        }

        showSelectView();
    }

    ui->label_caseName->setText(tempCase->getCaseName());
}

bool CWE_manage_simulation::verifyCaseAndSelect()
{
    if (tempCase == NULL)
    {
        return false;
    }

    CaseState theState = tempCase->getCaseState();
    if ((theState == CaseState::JOB_RUN)
            || (theState == CaseState::READY)
            || (theState == CaseState::OP_INVOKE))
    {
        myDriver->setCurrentCase(tempCase);
        return true;
    }
    return false;
}

QString CWE_manage_simulation::getStateText(StageState theState)
{
    if (theState == StageState::ERROR)
        return "*** ERROR ***";
    if (theState == StageState::FINISHED)
        return "Finished";
    if (theState == StageState::LOADING)
        return "Loading ...";
    if (theState == StageState::RUNNING)
        return "Running ...";
    if (theState == StageState::UNRUN)
        return "Not Yet Run";
    return "*** TOTAL ERROR ***";
}
