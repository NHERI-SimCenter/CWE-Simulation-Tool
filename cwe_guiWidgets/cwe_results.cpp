/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#include "cwe_results.h"
#include "ui_cwe_results.h"

#include "filemetadata.h"

#include "remoteFiles/fileoperator.h"
#include "remoteFiles/filetreenode.h"
#include "cwe_interfacedriver.h"
#include "cwe_globals.h"

#include "mainWindow/cwe_mainwindow.h"

#include "CFDanalysis/cweanalysistype.h"
#include "CFDanalysis/cwecaseinstance.h"
#include "CFDanalysis/cweresultinstance.h"

#include "visualUtils/resultVisuals/resultmesh3dwindow.h"
#include "visualUtils/resultVisuals/resultmesh2dwindow.h"
#include "visualUtils/resultVisuals/resultfield2dwindow.h"
#include "visualUtils/resultVisuals/resulttextdisp.h"

CWE_Results::CWE_Results(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Results)
{
    ui->setupUi(this);

    resultListModel = new QStandardItemModel(this);
    ui->resultsTreeView->setModel(resultListModel);

    resetViewInfo();
}

CWE_Results::~CWE_Results()
{
    delete ui;
}

void CWE_Results::linkMainWindow(CWE_MainWindow *theMainWin)
{
    CWE_Super::linkMainWindow(theMainWin);
    QObject::connect(theMainWindow, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()), Qt::QueuedConnection);

    QObject::connect(ui->resultsTreeView, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(resultViewClicked(QModelIndex)));
}

void CWE_Results::resetViewInfo()
{
    resultListModel->clear();
    resultCorrespondenceList.clear();
    while (!resultDirectory.isEmpty()) resultDirectory.takeLast()->deleteLater();

    QStringList HeaderList;
    HeaderList << "Result:" << "View?" << "Download?"  << "Type:" << "";
    resultListModel->setHorizontalHeaderLabels(HeaderList);

    ui->resultsTreeView->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->resultsTreeView->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(3,QHeaderView::Stretch);
    ui->resultsTreeView->header()->setSectionResizeMode(4,QHeaderView::ResizeToContents);

    loadingRow = true;
    resultListModel->appendRow(new QStandardItem("Loading Results . . ."));

    ui->label_theName->setText("Loading . . .");
    ui->label_theType->setText("Loading . . .");
    ui->label_theLocation->setText("Loading . . .");
}

void CWE_Results::changeVisibleState(cweResultInstance * toChange, ResultInstanceState newState)
{
    if (!resultDirectory.contains(toChange)) return;

    if ((newState == ResultInstanceState::NIL) || (newState == ResultInstanceState::UNLOADED))
    {
        if (!resultCorrespondenceList.contains(toChange)) return;

        int rowNum = resultCorrespondenceList.indexOf(toChange);
        resultListModel->removeRow(rowNum);
        resultCorrespondenceList.removeAt(rowNum);
    }
    else if (newState == ResultInstanceState::LOADED)
    {
        if (resultCorrespondenceList.contains(toChange)) return;
        int newRow = resultCorrespondenceList.size();
        resultCorrespondenceList.append(toChange);
        resultListModel->insertRow(newRow, toChange->getItemList());
    }

    if (loadingRow == false)
    {
        if (newState == ResultInstanceState::UNLOADED)
        {
            loadingRow = true;
            resultListModel->appendRow(new QStandardItem("Loading Results . . ."));
        }
        return;
    }

    for (cweResultInstance * aResult : resultDirectory)
    {
        if (aResult->getState() == ResultInstanceState::UNLOADED) return;
    }
    loadingRow = false;
    resultListModel->removeRow(resultListModel->rowCount() - 1);
}

FileNodeRef CWE_Results::getCaseFolder()
{
    return theMainWindow->getCurrentCase()->getCaseFolder();
}

CWEcaseInstance * CWE_Results::getCurrentCase()
{
    return theMainWindow->getCurrentCase();
}

void CWE_Results::on_downloadEntireCaseButton_clicked()
{
    if (theMainWindow->getCurrentCase() == nullptr)
    {
        return;
    }
    QString fileName = QFileDialog::getExistingDirectory(this, "Select Destination Folder:");
    if (fileName.isEmpty())
    {
        return;
    }

    theMainWindow->getCurrentCase()->downloadCase(fileName);
}

void CWE_Results::newCaseGiven()
{
    CWEcaseInstance * newCase = theMainWindow->getCurrentCase();
    resetViewInfo();

    if (newCase != nullptr)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)), Qt::QueuedConnection);
        newCaseState(newCase->getCaseState());
    }
}

void CWE_Results::resultViewClicked(QModelIndex modelID)
{
    if (!modelID.isValid()) return;
    CWEcaseInstance * currentCase = theMainWindow->getCurrentCase();
    if (currentCase == nullptr) return;
    int rowClicked = modelID.row();

    if ((modelID.column() != showCol) && (modelID.column() != downloadCol)) return;

    cweResultInstance * theResultInsance = resultCorrespondenceList.at(rowClicked);
    if (theResultInsance == nullptr) return;

    if (modelID.column() == showCol)
    {
        theResultInsance->enactShowOp();
    }
    else if (modelID.column() == downloadCol)
    {
        theResultInsance->enactDownloadOp();
    }
}

void CWE_Results::newCaseState(CaseState newState)
{
    if (resultDirectory.isEmpty()) populateResultDirectory();

    switch (newState)
    {
    case CaseState::DEFUNCT:
    case CaseState::ERROR:
    case CaseState::INVALID:
    case CaseState::OFFLINE:
        ui->downloadEntireCaseButton->setDisabled(true);
        break; //These states should be handled elsewhere
    case CaseState::DOWNLOAD:
    case CaseState::LOADING:
    case CaseState::EXTERN_OP:
    case CaseState::OP_INVOKE:
    case CaseState::PARAM_SAVE:
    case CaseState::RUNNING:
        ui->downloadEntireCaseButton->setDisabled(true);
        break;
    case CaseState::READY:
    case CaseState::READY_ERROR:
        ui->downloadEntireCaseButton->setEnabled(true);
        break;
    }
}

void CWE_Results::populateResultDirectory()
{
    CWEcaseInstance * currentCase = theMainWindow->getCurrentCase();
    if (currentCase == nullptr) return;
    if (currentCase->getCaseFolder().isNil()) return;
    CWEanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == nullptr) return;

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getDisplayName());
    ui->label_theLocation->setText(currentCase->getCaseFolder().getFullPath());

    QStringList stageNames = currentCase->getMyType()->getStageIds();

    for (QString aStage : stageNames)
    {
        QList<RESULT_ENTRY> resultList = currentCase->getMyType()->getStageFromId(aStage).resultList;

        for (RESULT_ENTRY aResult : resultList)
        {
            resultDirectory.append(new cweResultInstance(aStage, aResult, this));
        }
    }
}
