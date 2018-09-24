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

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

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
                     this, SLOT(newCaseGiven()));

    QObject::connect(ui->resultsTreeView, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(resultViewClicked(QModelIndex)));
}

void CWE_Results::resetViewInfo()
{
    resultListModel->clear();
    viewIsValid = false;

    QStringList HeaderList;
    HeaderList << "Loading Case Data, Please Wait";
    resultListModel->setHorizontalHeaderLabels(HeaderList);

    ui->resultsTreeView->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    showCol = -1;
    downloadCol = -1;
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
    CFDcaseInstance * newCase = theMainWindow->getCurrentCase();
    resetViewInfo();

    if (newCase != nullptr)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));
        newCaseState(newCase->getCaseState());
    }
}

void CWE_Results::resultViewClicked(QModelIndex modelID)
{
    QStandardItem * theItem = resultListModel->itemFromIndex(modelID);
    if (theItem == nullptr) return;
    CFDcaseInstance * currentCase = theMainWindow->getCurrentCase();
    if (currentCase == nullptr) return;

    if ((theItem->column() != showCol) && (theItem->column() != downloadCol))
    {
        return;
    }

    QString resultName = resultListModel->invisibleRootItem()->child(theItem->row(), 0)->text();
    if (resultName.isEmpty())
    {
        return;
    }

    RESULT_ENTRY resultObject = getResultObjectFromName(resultName);

    if (resultObject.type == "text")
    {
        if (theItem->column() == showCol)
        {
            ResultTextDisplay * resultPopup = new ResultTextDisplay(currentCase, &resultObject, nullptr);
            resultPopup->initializeView();;
        }
        else if (theItem->column() == downloadCol)
        {
            performSingleFileDownload(resultObject.file, resultObject.stage);
        }
    }
    else if (resultObject.type == "GLdata")
    {
        if (theItem->column() != showCol) return;

        ResultField2dWindow * resultPopup = new ResultField2dWindow(currentCase, &resultObject, nullptr);
        resultPopup->initializeView();
    }
    else if (resultObject.type == "GLmesh")
    {
        if (theItem->column() != showCol) return;

        ResultMesh2dWindow * resultPopup = new ResultMesh2dWindow(currentCase, &resultObject, nullptr);
        resultPopup->initializeView();
    }
    else if (resultObject.type == "GLmesh3D")
    {
        if (theItem->column() != showCol) return;

        ResultMesh3dWindow * resultPopup = new ResultMesh3dWindow(currentCase, &resultObject, nullptr);
        resultPopup->initializeView();
    }
    else if (resultObject.type == "GLmesh3D")
    {
        if (theItem->column() != downloadCol) return;

        performSingleFileDownload(resultObject.file, resultObject.stage);
    }
}

RESULT_ENTRY CWE_Results::getResultObjectFromName(QString name)
{
    RESULT_ENTRY ret;
    ret.type = "null";

    if (!viewIsValid) return ret;
    CFDcaseInstance * currentCase = theMainWindow->getCurrentCase();
    if (currentCase == nullptr) return ret;
    if (currentCase->getCaseFolder().isNil()) return ret;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == nullptr) return ret;

    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    for (auto itr = currentStates.constBegin(); itr != currentStates.constEnd(); itr++)
    {
        QString stageName = itr.key();
        StageState theStageState = currentStates.value(stageName, StageState::OFFLINE);
        if ((theStageState != StageState::FINISHED) &&
                (theStageState != StageState::FINISHED_PREREQ) &&
                (theStageState != StageState::ERROR))
        {
            continue;
        }

        QList<RESULT_ENTRY> resultList = currentCase->getMyType()->getStageFromId(itr.key()).resultList;

        for (RESULT_ENTRY aResult : resultList)
        {
            if (aResult.displayName  == name)
            {
                return aResult;
            }
        }
    }

    return ret;
}

void CWE_Results::newCaseState(CaseState newState)
{
    if (!viewIsValid) populateResultsScreen();
    if (!viewIsValid) return;

    switch (newState)
    {
    case CaseState::DEFUNCT:
    case CaseState::ERROR:
    case CaseState::INVALID:
    case CaseState::OFFLINE:
        resetViewInfo();
        ui->downloadEntireCaseButton->setDisabled(true);
        return; //These states should be handled elsewhere
    case CaseState::DOWNLOAD:
    case CaseState::LOADING:
    case CaseState::EXTERN_OP:
    case CaseState::OP_INVOKE:
    case CaseState::PARAM_SAVE:
        ui->downloadEntireCaseButton->setDisabled(true);
        resetViewInfo();
        return;
    case CaseState::RUNNING:
        ui->downloadEntireCaseButton->setDisabled(true);
        resetViewInfo();
        populateResultsScreen();
        return;
    case CaseState::READY:
    case CaseState::READY_ERROR:
        ui->downloadEntireCaseButton->setEnabled(true);
        resetViewInfo();
        populateResultsScreen();
        return;
    }
}

void CWE_Results::populateResultsScreen()
{
    if (viewIsValid) return;
    CFDcaseInstance * currentCase = theMainWindow->getCurrentCase();
    if (currentCase == nullptr) return;
    if (currentCase->getCaseFolder().isNil()) return;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == nullptr) return;

    viewIsValid = true;

    resultListModel->clear();
    QStringList HeaderList;
    HeaderList << "Result:" << "View?" << "Download?"  << "Type:";
    resultListModel->setHorizontalHeaderLabels(HeaderList);
    showCol = 1;
    downloadCol = 2;

    ui->resultsTreeView->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->resultsTreeView->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(3,QHeaderView::Stretch);

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getDisplayName());
    ui->label_theLocation->setText(currentCase->getCaseFolder().getFullPath());

    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    for (auto itr = currentStates.constBegin(); itr != currentStates.constEnd(); itr++)
    {
        QString stageName = itr.key();
        StageState theStageState = currentStates.value(stageName, StageState::OFFLINE);
        if ((theStageState != StageState::FINISHED) &&
                (theStageState != StageState::FINISHED_PREREQ) &&
                (theStageState != StageState::ERROR))
        {
            continue;
        }

        QList<RESULT_ENTRY> resultList = currentCase->getMyType()->getStageFromId(itr.key()).resultList;

        for (RESULT_ENTRY aResult : resultList)
        {
            if (aResult.type == "text")
            {
                addResult(aResult.displayName,true,true,"Data File");
            }
            else if (aResult.type == "GLdata")
            {
                addResult(aResult.displayName,true,false,"Flow Field Image");
            }
            else if (aResult.type == "GLmesh")
            {
                addResult(aResult.displayName,true,false,"Mesh Image");
            }
            else if (aResult.type == "GLmesh3D")
            {
                addResult(aResult.displayName,true,false,"3D Mesh Image");
            }
            else if (aResult.type == "download")
            {
                addResult(aResult.displayName,false,true,"Data Download");
            }
            else
            {
                cwe_globals::displayPopup("ERROR: Unrecognized result entry type in configuration.");
            }
        }
    }
}

void CWE_Results::performSingleFileDownload(QString filePathToDownload, QString stage)
{
    if (filePathToDownload.isEmpty()) return;
    QString localfileName = QFileDialog::getSaveFileName(this, "Save Downloaded File:");
    if (localfileName.isEmpty()) {return;}

    FileNodeRef targetNode = theMainWindow->getCurrentCase()->getCaseFolder().getChildWithName(stage);
    if (targetNode.isNil())
    {
        cwe_globals::displayPopup("The stage for this download has not been completed. Please check your case and try again.");
        return;
    }

    targetNode = cwe_globals::get_file_handle()->speculateFileWithName(targetNode, filePathToDownload, false);
    if (targetNode.isNil())
    {
        cwe_globals::displayPopup("The result to be downloaded does not exist. Please check your case and try again.");
        return;
    }
    cwe_globals::get_file_handle()->sendDownloadReq(targetNode, localfileName);
}

void CWE_Results::addResult(QString name, bool showeye, bool download, QString type)
{
    QList<QStandardItem *> List;

    QStandardItem *var1 = new QStandardItem(name);
    List.append(var1);

    QStandardItem *var2 = new QStandardItem;
    if (showeye) {
        var2->setIcon(QIcon(":/buttons/images/CWE_eye.png"));
    }
    else {
        var2->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    List.append(var2);

    QStandardItem *var3 = new QStandardItem;if (download) {
        var3->setIcon(QIcon(":/buttons/images/CWE_download.png"));
    }
    else {
        var3->setIcon(QIcon(":/buttons/images/CWE_empty.png"));
    }
    List.append(var3);

    QStandardItem *var4 = new QStandardItem(type);
    List.append(var4);

    // Populate our model
    resultListModel->appendRow(List);
}
