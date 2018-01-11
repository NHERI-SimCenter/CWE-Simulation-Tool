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

/*
 * the CWE_TabWidget is an extended version of a tab widget where
 * Tabs display a label AND a state
 * the data display area itself hold a standard QTabWidget, one tab per
 * variable group (as defined in the JSon config file)
 */

#include "cwe_tabwidget.h"
#include "ui_cwe_tabwidget.h"

#include "cwe_parampanel.h"
#include "cwe_stagestatustab.h"
#include "cwe_groupswidget.h"
#include "CFDanalysis/CFDcaseInstance.h"
#include <QString>
#include <QMap>
#include <QMapIterator>

#include "cwe_guiWidgets/cwe_parameters.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.h"

#include "../CFDClientProgram/vwtinterfacedriver.h"

#include "SimCenter_widgets/sctrmasterdatawidget.h"

CWE_TabWidget::CWE_TabWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_TabWidget)
{
    ui->setupUi(this);

    stageTabList = new QMap<QString, CWE_StageStatusTab *>();

    this->setButtonMode(CWE_BTN_NONE);
    this->setViewState(SimCenterViewState::visible);
}

CWE_TabWidget::~CWE_TabWidget()
{
    delete ui;
    delete stageTabList;
}

void CWE_TabWidget::setController(CWE_Parameters * newController)
{
    myController = newController;
}

void CWE_TabWidget::setViewState(SimCenterViewState state)
{
    foreach (QString stageName, m_viewState.keys())
    {
        switch (state)
        {
        case SimCenterViewState::editable:
            m_viewState[stageName] = SimCenterViewState::editable;
            break;
        case SimCenterViewState::hidden:
            m_viewState[stageName] = SimCenterViewState::hidden;
            break;
        case SimCenterViewState::visible:
        default:
            m_viewState[stageName] = SimCenterViewState::visible;
            state = SimCenterViewState::visible;
        }

        stageTabList->value(stageName)->getGroupsWidget()->setViewState(m_viewState.value(stageName));
    }
}

void CWE_TabWidget::setViewState(SimCenterViewState state, QString stageName)
{
    //TODO: PMH
    switch (state)
    {
    case SimCenterViewState::editable:
        m_viewState[stageName] = SimCenterViewState::editable;
        break;
    case SimCenterViewState::hidden:
        m_viewState[stageName] = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::visible:
    default:
        m_viewState[stageName] = SimCenterViewState::visible;
    }

    stageTabList->value(stageName)->getGroupsWidget()->setViewState(state);

    //Disable buttons (If appropriate)
}

SimCenterViewState CWE_TabWidget::viewState(QString stageName)
{
    return m_viewState.value(stageName);
}

void CWE_TabWidget::resetView()
{
    QMapIterator<QString, CWE_StageStatusTab *> iter(*stageTabList);

    while (iter.hasNext())
    {
        iter.next();
        /* remove the groupWidgets from ui->stagePanels */
        ui->stagePanels->removeWidget((iter.value())->groupWidget());
        /* delete the groupWidget */
        delete (iter.value())->groupWidget();
        /* delete the StageStatusTab */
        delete iter.value();
    }
    /* clear the stageTabList */
    stageTabList->clear();
}


void CWE_TabWidget::setParameterConfig(QJsonObject &obj)
{
    QVBoxLayout *tablayout = (QVBoxLayout *)ui->tabsBar->layout();
    delete tablayout;
    tablayout = new QVBoxLayout();
    tablayout->setMargin(0);
    tablayout->setSpacing(0);
    ui->tabsBar->setLayout(tablayout);

    stageTabList->clear();

    QJsonArray  sequence = obj.value(QString("sequence")).toArray();
    QJsonObject stages   = obj.value(QString("stages")).toObject();

    QMap<QString, StageState> stageStates;
    stageStates = myController->getDriver()->getCurrentCase()->getStageStates();

    foreach (QJsonValue theStage, sequence)
    {
        QString stageName = theStage.toString();
        QString stageLabel = stages.value(stageName).toObject().value("name").toString();
        stageLabel += "\nParameters";

        /* create a CWE_StageStatusTab */
        CWE_StageStatusTab *tab = new CWE_StageStatusTab(stageName, stageLabel, this);

        tab->setStatus(getStateText(stageStates.value(stageName)));

        tablayout->addWidget(tab);
        stageTabList->insert(stageName, tab);
        //QVBoxLayout *layout = (QVBoxLayout *)ui->tabsBar->layout();

        /* create a CWE_GroupsWidget */
        CWE_GroupsWidget *groupWidget = new CWE_GroupsWidget(this);
        ui->stagePanels->addWidget(groupWidget);

        /* link tab and groupWidget */
        tab->linkWidget(groupWidget);
        groupWidget->linkWidget(tab);

        /* set the parameter information for the CWE_GroupsWidget */
        groupWidget->setParameterConfig(stageName, obj);

        /* connect signals and slots */
        QObject::connect(tab,SIGNAL(btn_pressed(CWE_GroupsWidget *)),this,SLOT(on_groupTabSelected(CWE_GroupsWidget *)));
        QObject::connect(tab,SIGNAL(btn_activated(CWE_StageStatusTab*)),this,SLOT(on_tabActivated(CWE_StageStatusTab *)));
    }

    tablayout->addSpacerItem(new QSpacerItem(10,40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    initQuickParameterPtr();

    QString firstTabKey = sequence[0].toString();
    if (firstTabKey != "") { stageTabList->value(firstTabKey)->setActive(); }

    setButtonMode(CWE_BTN_RUN);
}

void CWE_TabWidget::updateParameterValues(QMap<QString, QString> newValues)
{
    foreach (QString stageName, stageTabList->keys())
    {
        stageTabList->value(stageName)->getGroupsWidget()->updateParameterValues(newValues);
    }
}

void CWE_TabWidget::initQuickParameterPtr()
{
    foreach (QString stageName, stageTabList->keys())
    {
        stageTabList->value(stageName)->getGroupsWidget()->initQuickParameterPtr();
    }
}

void CWE_TabWidget::on_pbtn_run_clicked()
{
    myController->performCaseCommand(getCurrentSelectedStage(), CaseCommand::RUN);

    setViewState(SimCenterViewState::visible);
    setButtonMode(CWE_BTN_CANCEL);
}

QMap<QString, QString> CWE_TabWidget::collectParamData()
{
    QMap<QString, QString> currentParameters;

    foreach (QString stageName, stageTabList->keys())
    {
        stageTabList->value(stageName)->getGroupsWidget()->collectParamData(currentParameters);
    }

    qDebug() << currentParameters;

    return currentParameters;
}

void CWE_TabWidget::on_pbtn_cancel_clicked()
{
    myController->performCaseCommand(getCurrentSelectedStage(), CaseCommand::CANCEL);

    //setButtonMode(CWE_BTN_RUN|CWE_BTN_CANCEL|CWE_BTN_RESULTS|CWE_BTN_ROLLBACK);

    setViewState(SimCenterViewState::editable);
    setButtonMode(CWE_BTN_RUN);
}

void CWE_TabWidget::on_pbtn_results_clicked()
{
    myController->switchToResults();
}

void CWE_TabWidget::on_pbtn_rollback_clicked()
{
    myController->performCaseCommand(getCurrentSelectedStage(), CaseCommand::ROLLBACK);
}

QString CWE_TabWidget::getStateText(StageState theState)
{
    QString msg = "*** TOTAL ERROR ***";
    if (theState == StageState::ERROR)         { msg = "*** ERROR ***"; }
    else if (theState == StageState::FINISHED) { msg = "Task Finished"; }
    else if (theState == StageState::LOADING)  { msg = "Loading Data ..."; }
    else if (theState == StageState::RUNNING)  { msg = "Task Running"; }
    else if (theState == StageState::UNRUN)    { msg = "Not Yet Run"; }
    return msg;
}

QString CWE_TabWidget::getCurrentSelectedStage()
{
    QString theStage = "UNKNOWN";

    foreach (CWE_StageStatusTab *tab, *stageTabList)
    {
        if (tab->isActiveWindow()) {
            theStage = tab->getStageKey();
            break;
        }
    }

    return theStage;
}

void CWE_TabWidget::setButtonMode(uint mode)
{
    bool btnState;

    btnState = (mode & CWE_BTN_RUN)?true:false;
    ui->pbtn_run->setEnabled(btnState);

    btnState = (mode & CWE_BTN_CANCEL)?true:false;
    ui->pbtn_cancel->setEnabled(btnState);

    btnState = (mode & CWE_BTN_RESULTS)?true:false;
    ui->pbtn_results->setEnabled(btnState);

    btnState = (mode & CWE_BTN_ROLLBACK)?true:false;
    ui->pbtn_rollback->setEnabled(btnState);
}


/* *** SLOTS *** */

void CWE_TabWidget::on_groupTabSelected(CWE_GroupsWidget *groupWidget)
{
    /* activate the proper panel */
    ui->stagePanels->setCurrentWidget(groupWidget);
}

void CWE_TabWidget::on_tabActivated(CWE_StageStatusTab *activeTabWidget)
{
    foreach (QString key, stageTabList->keys())
    {
        stageTabList->value(key)->setInActive();
    }
    activeTabWidget->setActive();
}
