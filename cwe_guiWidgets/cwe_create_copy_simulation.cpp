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

#include "cwe_create_copy_simulation.h"
#include "ui_cwe_create_copy_simulation.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"
#include "cwe_interfacedriver.h"

#include "cwe_globals.h"

CWE_Create_Copy_Simulation::CWE_Create_Copy_Simulation(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Create_Copy_Simulation)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);
}

CWE_Create_Copy_Simulation::~CWE_Create_Copy_Simulation()
{
    delete ui;
}

void CWE_Create_Copy_Simulation::linkDriver(CWE_InterfaceDriver * theDriver)
{
    CWE_Super::linkDriver(theDriver);
    this->populateCaseTypes();
    if (!theDriver->inOfflineMode())
    {
        ui->primary_remoteFileTree->setupFileView();
        ui->secondary_remoteFileTree->setupFileView();
    }
}

void CWE_Create_Copy_Simulation::on_pBtn_create_copy_clicked()
{
    if (myDriver->inOfflineMode())
    {
        myDriver->createNewCase(selectedTemplate);
        myDriver->getMainWindow()->switchToParameterTab();
        return;
    }

    /* take emergency exit if nothing has been selected */
    FileNodeRef selectedNode = ui->primary_remoteFileTree->getSelectedFile();
    if (selectedNode.isNil())
    {
        cwe_globals::displayPopup("Please select a folder to place the new case.");
        return;
    }
    if (selectedNode.getFileType() != FileType::DIR)
    {
        cwe_globals::displayPopup("Please select a folder to place the new case.");
        return;
    }

    /* OK, something has been selected */
    CFDcaseInstance * newCase;

    QString newCaseName = ui->lineEdit_newCaseName->text();

    if (!cwe_globals::isValidFolderName(newCaseName))
    {
        cwe_globals::displayPopup("Please input a valid folder name");
        return;
    }

    if (ui->tabWidget->currentWidget() == ui->tab_NewCase)
    {
        /* we are creating a new case */

        if (selectedTemplate == NULL)
        {
            cwe_globals::displayPopup("Please select a valid case type.");
            return;
        }
        newCase = myDriver->createNewCase(selectedTemplate);
        if (!newCase->createCase(newCaseName, selectedNode))
        {
            cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
            return;
        }
    }
    else
    {
        /* we are cloning from an existing case */

        FileNodeRef secondNode = ui->secondary_remoteFileTree->getSelectedFile();
        if (selectedNode.isNil())
        {
            cwe_globals::displayPopup("Please select a folder to duplicate.");
            return;
        }
        if (selectedNode.getFileType() != FileType::DIR)
        {
            cwe_globals::displayPopup("Please select a folder to duplicate.");
            return;
        }
        CFDcaseInstance * tempCase = myDriver->getCaseFromFolder(secondNode);
        CaseState dupState = tempCase->getCaseState();

        if (dupState == CaseState::INVALID)
        {
            cwe_globals::displayPopup("ERROR: Can only duplicate CFD cases managed by CWE. Please select a valid folder containing a case for duplication.");
            return;
        }
        if (dupState == CaseState::LOADING)
        {
            cwe_globals::displayPopup("Please wait for case folder to load before attempting to duplicate.");
            return;
        }
        if (dupState != CaseState::READY)
        {
            cwe_globals::displayPopup("Unable to duplicate case. Please check that the case does not have an active job.");
            return;
        }

        newCase = myDriver->createNewCase(NULL);
        if (!newCase->duplicateCase(newCaseName, selectedNode, secondNode))
        {
            cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
            return;
        }
    }

    if (newCase->getCaseState() != CaseState::OP_INVOKE)
    {
        cwe_globals::displayPopup("Cannot create new case due to internal error.");
        newCase->deleteLater();
        return;
    }

    //Set new case will signal the other panels so that they can get configurations
    myDriver->setCurrentCase(newCase);

    /* time to switch to the ParameterTab */
    myDriver->getMainWindow()->switchToParameterTab();
}

void CWE_Create_Copy_Simulation::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0: // create new case
        ui->pBtn_create_copy->setText("Create New Simulation");
        break;
    case 1: // duplicate an existing case
        ui->pBtn_create_copy->setText(tr("Duplicate && Edit"));
        break;
    default:
        // this one should not happen.
        ui->tabWidget->setCurrentIndex(0);
    }
}

void CWE_Create_Copy_Simulation::populateCaseTypes()
{
    QList<CFDanalysisType *> * templateList = myDriver->getTemplateList();
    QGridLayout *layout = new QGridLayout();

    int idx = 0;

    foreach (CFDanalysisType * caseType, *templateList) {
        /* get analysis type and name info from JSON */
        QJsonDocument * configuration = caseType->getRawConfig();

        QJsonObject confObj = configuration->object();
        QString theName = confObj["name"].toString();
        QString theDescription = confObj["description"].toString();

        /* create UI selection block */
        QRadioButton *radioBtn = new QRadioButton(theName, ui->scroll_NewCase);
        QPushButton *buttonIcon = new QPushButton(ui->scroll_NewCase);

        buttonIcon->setIcon(*(caseType->getIcon()));
        buttonIcon->setIconSize(QSize(150,100));
        buttonIcon->setMinimumSize(150, 100);
        buttonIcon->setMaximumSize(150, 100);
        QLabel *labelDescription = new QLabel(ui->scroll_NewCase);
        if (theDescription == "") {
            theDescription = "some\ndescription\nof this\ncase.";
        }
        labelDescription->setText(theDescription);

        int cnt = layout->rowCount();
        layout->addWidget(buttonIcon,cnt+1,1,1,1);
        layout->addWidget(labelDescription,cnt+1,2,1,1);
        layout->addWidget(radioBtn,cnt,1,1,2);

        CASE_TYPE_DATA list;
        list.radioBtn = radioBtn;
        list.pbtn     = buttonIcon;
        list.templateData = caseType;
        caseTypeDataList.append(list);

        /* create appropriate connection between signals and slots */

        QObject::connect(buttonIcon, SIGNAL(pressed()),       this, SLOT(selectCaseTemplate()));
        QObject::connect(radioBtn, SIGNAL(toggled(bool)),     this, SLOT(selectCaseTemplate()));

        idx++;
    }

    QLayout *lyt = ui->scroll_NewCase->layout();
    if (lyt != NULL) {delete lyt;}

    ui->scroll_NewCase->setLayout(layout);
}

void CWE_Create_Copy_Simulation::selectCaseTemplate()
{
    QObject *sender = QObject::sender();
    QVector<CASE_TYPE_DATA>::iterator i;

    for (i = caseTypeDataList.begin(); i != caseTypeDataList.end(); i++) {
        if  (i->pbtn == (QPushButton *)sender) {
            i->radioBtn->setChecked(true);

            selectedTemplate = i->templateData;

            break;
        }
        else if  (i->radioBtn == (void *)sender ) {
            /* QRadioButton toggled -- Qt is taking care of this */
            //i->radioBtn->setChecked(true);

            /* initiate case */
            selectedTemplate = i->templateData;

            break;
        }
        else {
            /* turn off not selected QRadioButton -- Qt is taking care of this */
            //((QRadioButton *)(*i)[0])->setChecked(false);
        }
    }
}
