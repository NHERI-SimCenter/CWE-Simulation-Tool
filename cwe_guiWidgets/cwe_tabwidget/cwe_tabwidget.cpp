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
#include "../CFDClientProgram/vwtinterfacedriver.h"

#include "SimCenter_widgets/sctrmasterdatawidget.h"

CWE_TabWidget::CWE_TabWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_TabWidget)
{
    ui->setupUi(this);

    stageTabList = new QMap<QString, CWE_StageStatusTab *>();
    quickParameterPtr = new QMap<QString, SCtrMasterDataWidget *>();

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
    switch (state)
    {
    case SimCenterViewState::editable:
        m_viewState = SimCenterViewState::editable;
        break;
    case SimCenterViewState::hidden:
        m_viewState = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::visible:
    default:
        m_viewState = SimCenterViewState::visible;
    }
}

SimCenterViewState CWE_TabWidget::viewState()
{
    return m_viewState;
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
        CWE_StageStatusTab *tab = new CWE_StageStatusTab(stageLabel, this);

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
        QObject::connect(tab,SIGNAL(btn_pressed(CWE_GroupsWidget *,QString)),this,SLOT(on_groupTabSelected(CWE_GroupsWidget *, QString)));
        QObject::connect(tab,SIGNAL(btn_activated(CWE_StageStatusTab*)),this,SLOT(on_tabActivated(CWE_StageStatusTab *)));
        //QObject::connect(tab,SIGNAL(btn_released(CWE_GroupsWidget *)),this,SLOT(on_groupTabSelected(CWE_GroupsWidget *)));
    }

    tablayout->addSpacerItem(new QSpacerItem(10,40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    setButtonMode(CWE_BTN_RUN);
}


void CWE_TabWidget::initQuickParameterPtr()
{
    SCtrMasterDataWidget *ptr = NULL;
    QString key;

    quickParameterPtr->clear();

    QMapIterator<QString, CWE_StageStatusTab *> stageTabIter(*stageTabList);
    while (stageTabIter.hasNext())
    {
        stageTabIter.next();
        QMap<QString, SCtrMasterDataWidget *> groupMap = (stageTabIter.value())->groupWidget()->getParameterWidgetMap();
        QMapIterator<QString, SCtrMasterDataWidget *> groupIter(groupMap);
        while (groupIter.hasNext())
        {
            groupIter.next();
            quickParameterPtr->insert(groupIter.key(), groupIter.value());
        }
    }
}

void CWE_TabWidget::on_pbtn_run_clicked()
{
    myController->performCaseCommand(currentSelectedStage, CaseCommand::RUN);

    setButtonMode(CWE_BTN_CANCEL);
}

QMap<QString, QString> CWE_TabWidget::collectParamData()
{
    QString val;
    QMap<QString, QString> currentParameters;

    // collect parameter values from all SCtrMasterDataWidget objects
    QMapIterator<QString, SCtrMasterDataWidget *> iter(*quickParameterPtr);

    while (iter.hasNext())
    {
        iter.next();

        currentParameters.insert(iter.key(), (iter.value())->value());
    }

    return currentParameters;
}

void CWE_TabWidget::on_pbtn_cancel_clicked()
{
    myController->performCaseCommand(currentSelectedStage, CaseCommand::CANCEL);

    //setButtonMode(CWE_BTN_RUN|CWE_BTN_CANCEL|CWE_BTN_RESULTS|CWE_BTN_ROLLBACK);
    setButtonMode(CWE_BTN_RUN);
}

void CWE_TabWidget::on_pbtn_results_clicked()
{
    myController->switchToResults();
}

void CWE_TabWidget::on_pbtn_rollback_clicked()
{
    myController->performCaseCommand(currentSelectedStage, CaseCommand::ROLLBACK);
}

QString CWE_TabWidget::getStateText(StageState theState)
{
    if (theState == StageState::ERROR)
        return "*** ERROR ***";
    if (theState == StageState::FINISHED)
        return "Task Finished";
    if (theState == StageState::LOADING)
        return "Loading Data ...";
    if (theState == StageState::RUNNING)
        return "Task Running";
    if (theState == StageState::UNRUN)
        return "Not Yet Run";
    return "*** TOTAL ERROR ***";
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

void CWE_TabWidget::on_groupTabSelected(CWE_GroupsWidget *groupWidget, QString s)
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
