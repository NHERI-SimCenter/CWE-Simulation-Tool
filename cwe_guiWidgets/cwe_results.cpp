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

#include "vwtinterfacedriver.h"
#include "cwe_globals.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "cwe_result_popup.h"

CWE_Results::CWE_Results(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Results)
{
    ui->setupUi(this);

    model = new QStandardItemModel(this);
    ui->resultsTreeView->setModel(model);

    resetViewInfo();
}

CWE_Results::~CWE_Results()
{
    delete ui;
}

void CWE_Results::linkDriver(VWTinterfaceDriver * newDriver)
{
    CWE_Super::linkDriver(newDriver);
    QObject::connect(myDriver, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()));

    QObject::connect(ui->resultsTreeView, SIGNAL(clicked(QModelIndex)),
                     this, SLOT(resultViewClicked(QModelIndex)));
}

void CWE_Results::resetViewInfo()
{
    model->clear();
    viewIsValid = false;

    QStringList HeaderList;
    HeaderList << "Loading Case Data, Please Wait";
    model->setHorizontalHeaderLabels(HeaderList);

    ui->resultsTreeView->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    showCol = -1;
    downloadCol = -1;
}

void CWE_Results::on_downloadEntireCaseButton_clicked()
{
    if (myDriver->getCurrentCase() == NULL)
    {
        return;
    }
    QString fileName = QFileDialog::getExistingDirectory(this, "Select Destination Folder:");
    if (fileName.isEmpty())
    {
        return;
    }

    myDriver->getCurrentCase()->downloadCase(fileName);
}

void CWE_Results::newCaseGiven()
{
    CFDcaseInstance * newCase = myDriver->getCurrentCase();
    resetViewInfo();

    if (newCase != NULL)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));
        newCaseState(newCase->getCaseState());
    }
}

void CWE_Results::resultViewClicked(QModelIndex modelID)
{
    QStandardItem * theItem = model->itemFromIndex(modelID);
    if (theItem == NULL) return;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return;

    if (!(theItem->column() == showCol) && !(theItem->column() == downloadCol))
    {
        return;
    }

    QString resultName = model->invisibleRootItem()->child(theItem->row(), 0)->text();
    if (resultName.isEmpty())
    {
        return;
    }

    QMap<QString, QString> resultObject = getResultObjectFromName(resultName);
    if (resultObject.isEmpty())
    {
        return;
    }

    QString resultType = resultObject.value("type");
    if (resultType == "text")
    {
        CWE_Result_Popup * resultPopup = NULL;
        if (theItem->column() == showCol)
        {
            resultPopup = new CWE_Result_Popup(currentCase->getCaseName(),
                                 currentCase->getMyType()->getName(),
                                 resultObject, myDriver);
        }
        else if (theItem->column() == downloadCol)
        {
            resultPopup = new CWE_Result_Popup(currentCase->getCaseName(),
                                 currentCase->getMyType()->getName(),
                                 resultObject, myDriver, true);
        }
        resultPopup->show();
    }
    else if (resultType == "GLdata")
    {
        if (theItem->column() != showCol)
        {
            return;
        }
        CWE_Result_Popup * resultPopup = new CWE_Result_Popup(currentCase->getCaseName(),
                             currentCase->getMyType()->getName(),
                             resultObject, myDriver);
        resultPopup->show();
    }
    else if (resultType == "GLmesh")
    {
        if (theItem->column() != showCol)
        {
            return;
        }
        CWE_Result_Popup * resultPopup = new CWE_Result_Popup(currentCase->getCaseName(),
                             currentCase->getMyType()->getName(),
                             resultObject, myDriver);
        resultPopup->show();
    }
}

QMap<QString, QString> CWE_Results::getResultObjectFromName(QString name)
{
    QMap<QString, QString> ret;

    if (!viewIsValid) return ret;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return ret;
    if (currentCase->getCaseFolder().isEmpty()) return ret;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == NULL) return ret;
    QJsonObject configobj    = theTemplate->getRawConfig()->object();
    QJsonObject stagesobj = configobj.value("stages").toObject();

    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    for (auto itr = stagesobj.constBegin(); itr != stagesobj.constEnd(); itr++)
    {
        QString stageName = itr.key();
        StageState theStageState = currentStates.value(stageName, StageState::ERROR);
        if ((theStageState != StageState::FINISHED) &&
                (theStageState != StageState::FINISHED_PREREQ))
        {
            continue;
        }

        QJsonArray resultArray = (*itr).toObject().value("results").toArray();
        for (auto itr2 = resultArray.constBegin(); itr2 != resultArray.constEnd(); itr2++)
        {
            QJsonObject aResult = (*itr2).toObject();
            if (aResult.value("name").toString() == name)
            {
                for (auto itr3 = aResult.constBegin(); itr3 != aResult.constEnd(); itr3++)
                {
                    ret.insert(itr3.key(),(*itr3).toString());
                    ret.insert("stage",stageName);
                }

                return ret;
            }
        }
    }
    return ret;
}

void CWE_Results::newCaseState(CaseState newState)
{
    if (!viewIsValid)
    {
        populateResultsScreen();
    }

    if (!viewIsValid)
    {
        return;
    }

    switch (newState)
    {
    case CaseState::DEFUNCT:
    case CaseState::ERROR:
    case CaseState::INVALID:
    case CaseState::OFFLINE:
        resetViewInfo();
        return; //These states should be handled elsewhere
        break;
    case CaseState::DOWNLOAD:
    case CaseState::LOADING:
    case CaseState::OP_INVOKE:
        resetViewInfo();
        return;
        break;
    case CaseState::RUNNING:
    case CaseState::READY:
        resetViewInfo();
        populateResultsScreen();
        return;
        break;
    default:
        myDriver->fatalInterfaceError("Remote case has unhandled state");
        return;
        break;
    }
}

void CWE_Results::populateResultsScreen()
{
    if (viewIsValid) return;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return;
    if (currentCase->getCaseFolder().isEmpty()) return;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == NULL) return;
    QJsonObject configobj    = theTemplate->getRawConfig()->object();
    QJsonObject stagesobj = configobj.value("stages").toObject();

    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    viewIsValid = true;

    model->clear();
    QStringList HeaderList;
    HeaderList << "Result:" << "View?" << "Download?"  << "Type:";
    model->setHorizontalHeaderLabels(HeaderList);
    showCol = 1;
    downloadCol = 2;

    ui->resultsTreeView->header()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->resultsTreeView->header()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    ui->resultsTreeView->header()->setSectionResizeMode(3,QHeaderView::Stretch);

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getName());
    ui->label_theLocation->setText(currentCase->getCaseFolder());

    for (auto itr = stagesobj.constBegin(); itr != stagesobj.constEnd(); itr++)
    {
        QString stageName = itr.key();
        StageState theStageState = currentStates.value(stageName, StageState::ERROR);
        if ((theStageState != StageState::FINISHED) &&
                (theStageState != StageState::FINISHED_PREREQ))
        {
            continue;
        }

        QJsonArray resultArray = (*itr).toObject().value("results").toArray();
        for (auto itr2 = resultArray.constBegin(); itr2 != resultArray.constEnd(); itr2++)
        {
            QJsonObject aResult = (*itr2).toObject();
            QString resultType = aResult.value("type").toString();
            if (resultType == "text")
            {
                addResult(aResult.value("name").toString(),true,true,"Data File");
            }
            else if (resultType == "GLdata")
            {
                addResult(aResult.value("name").toString(),true,false,"Flow Field Image");
            }
            else if (resultType == "GLmesh")
            {
                addResult(aResult.value("name").toString(),true,false,"Mesh Image");
            }
            else
            {
                cwe_globals::displayPopup("ERROR: Unrecognized result entry type in configuration.");
            }
        }
    }
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
    model->appendRow(List);
}
