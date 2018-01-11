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

#include "cwe_parameters.h"
#include "ui_cwe_parameters.h"

#include "cwe_tabwidget/cwe_parampanel.h"

#include "vwtinterfacedriver.h"
#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.h"

#include "SimCenter_widgets/sctrstates.h"

CWE_Parameters::CWE_Parameters(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Parameters)
{
    ui->setupUi(this);
    ui->theTabWidget->setController(this);
}

CWE_Parameters::~CWE_Parameters()
{
    delete ui;
}

void CWE_Parameters::linkDriver(VWTinterfaceDriver * newDriver)
{
    CWE_Super::linkDriver(newDriver);
    QObject::connect(myDriver, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()));
}

void CWE_Parameters::resetViewInfo()
{
    paramWidgetsExist = false;

    // erase all stage tabs
    ui->theTabWidget->resetView();
}

void CWE_Parameters::on_pbtn_saveAllParameters_clicked()
{
    saveAllParams();
}

void CWE_Parameters::saveAllParams()
{
    CFDcaseInstance * linkedCFDCase = myDriver->getCurrentCase();
    if (linkedCFDCase != NULL)
    {
        linkedCFDCase->changeParameters(ui->theTabWidget->collectParamData());
    }
}

void CWE_Parameters::newCaseGiven()
{
    CFDcaseInstance * newCase = myDriver->getCurrentCase();

    this->resetViewInfo();

    if (newCase != NULL)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));

        if (newCase->getMyType() == NULL) return;

        createUnderlyingParamWidgets();
    }
}

void CWE_Parameters::newCaseState(CaseState newState)
{
    if (!paramWidgetsExist)
    {
        createUnderlyingParamWidgets();
    }

    if (!paramWidgetsExist)
    {
        return;
    }

    switch (newState)
    {
    case CaseState::DEFUNCT:
    case CaseState::ERROR:
    case CaseState::INVALID:
    case CaseState::OFFLINE:
        ui->theTabWidget->setViewState(SimCenterViewState::hidden);
        ui->theTabWidget->setButtonMode(SimCenterButtonMode_NONE);
        return; //These states should be handled elsewhere
        break;
    case CaseState::LOADING:
        ui->theTabWidget->setViewState(SimCenterViewState::visible);
        ui->theTabWidget->setButtonMode(SimCenterButtonMode_NONE);
        break;
    case CaseState::OP_INVOKE:
        ui->theTabWidget->setViewState(SimCenterViewState::visible);
        ui->theTabWidget->setButtonMode(SimCenterButtonMode_NONE);
        break;
    case CaseState::JOB_RUN:
        setVisibleAccordingToStage();
        setButtonsAccordingToStage();
        break;
    case CaseState::READY:
        ui->theTabWidget->updateParameterValues(myDriver->getCurrentCase()->getCurrentParams());
        setVisibleAccordingToStage();
        setButtonsAccordingToStage();
        break;
    default:
        myDriver->fatalInterfaceError("Remote case has unhandled state");
        return;
        break;
    }
}

void CWE_Parameters::setButtonsAccordingToStage()
{
    QMap<QString, StageState> stageStates = myDriver->getCurrentCase()->getStageStates();
    for (auto itr = stageStates.cbegin(); itr != stageStates.cend(); itr++)
    {
        ui->theTabWidget->setTabStage(*itr, itr.key());
        switch (*itr)
        {
        case StageState::LOADING:
        case StageState::ERROR:
            ui->theTabWidget->setButtonMode(SimCenterButtonMode_NONE, itr.key());
            break;
        case StageState::RUNNING:
            ui->theTabWidget->setButtonMode(SimCenterButtonMode_CANCEL, itr.key());
            break;
        case StageState::FINISHED:
            ui->theTabWidget->setButtonMode(SimCenterButtonMode_RESET, itr.key());
            break;
        case StageState::UNRUN:
        default:
            ui->theTabWidget->setButtonMode(SimCenterButtonMode_RUN, itr.key());
        }
    }
}

void CWE_Parameters::setVisibleAccordingToStage()
{
    QMap<QString, StageState> stageStates = myDriver->getCurrentCase()->getStageStates();
    for (auto itr = stageStates.cbegin(); itr != stageStates.cend(); itr++)
    {
        switch (*itr)
        {
        case StageState::ERROR:
        case StageState::RUNNING:
        case StageState::FINISHED:
        case StageState::LOADING:
            ui->theTabWidget->setViewState(SimCenterViewState::visible, itr.key());
            break;
        case StageState::UNRUN:
        default:
            ui->theTabWidget->setViewState(SimCenterViewState::editable, itr.key());
        }
    }
}

void CWE_Parameters::createUnderlyingParamWidgets()
{
    if (paramWidgetsExist) return;

    CFDcaseInstance * newCase = myDriver->getCurrentCase();

    if (newCase == NULL) return;
    if (newCase->getMyType() == NULL) return;
    if (newCase->getCaseFolder().isEmpty()) return;

    QJsonObject rawConfig = newCase->getMyType()->getRawConfig()->object();

    ui->label_theName->setText(newCase->getCaseName());
    ui->label_theType->setText(newCase->getMyType()->getName());
    ui->label_theLocation->setText(newCase->getCaseFolder());

    ui->theTabWidget->setParameterConfig(rawConfig);
    ui->theTabWidget->setViewState(SimCenterViewState::visible);

    paramWidgetsExist = true;
}

void CWE_Parameters::switchToResults()
{
    myDriver->getMainWindow()->switchToResultsTab();
}

void CWE_Parameters::performCaseCommand(QString stage, CaseCommand toEnact)
{
    if (myDriver->getCurrentCase() == NULL)
    {
        return;
    }

    //TODO: Check that commands are valid : PRS

    if (toEnact == CaseCommand::CANCEL)
    {
        myDriver->getCurrentCase()->stopJob(stage);
    }
    else if (toEnact == CaseCommand::ROLLBACK)
    {
        myDriver->getCurrentCase()->rollBack(stage);
    }
    else if (toEnact == CaseCommand::RUN)
    {
        myDriver->getCurrentCase()->startStageApp(stage);
    }
}
