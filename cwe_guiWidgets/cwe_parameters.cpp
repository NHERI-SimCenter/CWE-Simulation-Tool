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
        return; //These states should be handled elsewhere
        break;
    case CaseState::LOADING:
    case CaseState::OP_INVOKE:
        ui->theTabWidget->setViewState(SimCenterViewState::visible);
        break;
    case CaseState::JOB_RUN:
        setVisibleAccordingToStage();
        break;
    case CaseState::READY:
        ui->theTabWidget->updateParameterValues(myDriver->getCurrentCase()->getCurrentParams());
        setVisibleAccordingToStage();
        break;
    default:
        myDriver->fatalInterfaceError("Remote case has unhandled state");
        return;
        break;
    }
}

void CWE_Parameters::setVisibleAccordingToStage()
{
    QMap<QString, StageState> stageStates = myDriver->getCurrentCase()->getStageStates();
    //TODO: PMH
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
