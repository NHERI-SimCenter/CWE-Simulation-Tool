#include "cwe_parameters.h"
#include "ui_cwe_parameters.h"

#include "cwe_tabwidget/cwe_parampanel.h"

#include "vwtinterfacedriver.h"
#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.h"

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
    viewIsValid = false;

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
    //TODO: implement functions for changes in current params or stage states

    if (!viewIsValid)
    {
        createUnderlyingParamWidgets();
    }
}

void CWE_Parameters::createUnderlyingParamWidgets()
{
    if (viewIsValid) return;

    CFDcaseInstance * newCase = myDriver->getCurrentCase();

    if (newCase == NULL) return;
    if (newCase->getMyType() == NULL) return;

    QJsonObject rawConfig = newCase->getMyType()->getRawConfig()->object();

    ui->label_theName->setText(newCase->getCaseName());
    ui->label_theType->setText(newCase->getMyType()->getName());
    ui->label_theLocation->setText(newCase->getCaseFolder());

    ui->theTabWidget->setParameterConfig(rawConfig);

    viewIsValid = true;
}

void CWE_Parameters::switchToResults()
{
    myDriver->getMainWindow()->switchToResultsTab();
}

void CWE_Parameters::performCaseCommand(QString stage, CaseCommand toEnact)
{
    //TODO: link commands with active case
    //TODO: Check that commands are valid
}
