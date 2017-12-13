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

void CWE_Parameters::initStateTabs()
{
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == NULL) return;
    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getName());
    ui->label_theLocation->setText(currentCase->getCaseFolder());


    // get the current configuration file
    QJsonObject   obj    = theTemplate->getRawConfig()->object();

    // read stage list for the current problem
    QJsonObject    stages     = obj["stages"].toObject();
    QList<QString> stageNames = stages.keys();

    // create stage tabs
    foreach (QString name, stageNames)
    {
        // create the stage tab
        /* not sure about that yet ... */

        //m_stageTabs[name] = new CWE_StageTab(parent=this);

        QJsonObject stageInfo = stages[name].toObject();
        QString labelText;

        labelText = stageInfo["name"].toString();
        labelText = labelText.append("\nParameters");

        ///ui->theTabWidget-> ... m_stageTabs[name]->setData(currentStates[name]);

    }

#if 0
    int cnt = 0;

    foreach (QString name, stageNames)
    {
        QJsonObject stageInfo = stages[name].toObject();
        QString labelText;

        labelText = stageInfo["name"].toString();
        labelText = labelText.append("\nParameters");

        // add a stage tab to ui->theTabWidget
        int idx = ui->theTabWidget->addGroupTab(name, labelText, currentStates[name]);
        stageTabsIndex.insert(name, idx);

        // add varGroub tabs
        QJsonArray theGroups = stageInfo["groups"].toArray();
        foreach (const QJsonValue item, theGroups)
        {
            QString subTitle = item.toString();
            QJsonArray varList = varGroups[subTitle].toArray();
            ui->theTabWidget->addVarTab(name, subTitle, &varList, &vars, &setVars);
        }

        cnt++;
    }

    if (cnt>0) {ui->theTabWidget->setIndex(0);}
    // ----
#endif

    viewIsValid = true;

    //??? ui->theTabWidget->
}

void CWE_Parameters::resetViewInfo()
{
    viewIsValid = false;

    // erase all stage tabs
    ui->theTabWidget->resetView();

    this->initStateTabs();
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

    if (newCase != NULL)
    {
        QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                         this, SLOT(newCaseState(CaseState)));
    }

    resetViewInfo();
}

void CWE_Parameters::newCaseState(CaseState newState)
{
    //TODO: implement functions for changes in current params or stage states
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
