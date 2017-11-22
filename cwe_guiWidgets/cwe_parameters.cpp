#include "cwe_parameters.h"
#include "ui_cwe_parameters.h"
#include "cwe_tabwidget/cwe_parametertab.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QLineEdit>

#include "qdebug.h"

#include "vwtinterfacedriver.h"
#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "cwe_guiWidgets/cwe_tabwidget/cwe_stagetab.h"

CWE_Parameters::CWE_Parameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Parameters)
{
    ui->setupUi(this);

    QObject::connect(ui->theTabWidget, SIGNAL(switchToParameterTab()), this, SLOT(switchToParameterSlot()));
    QObject::connect(ui->theTabWidget, SIGNAL(switchToCreateTab()),    this, SLOT(switchToCreateSlot())   );
    QObject::connect(ui->theTabWidget, SIGNAL(switchToResultsTab()),   this, SLOT(switchToResultsSlot())  );

    }

CWE_Parameters::~CWE_Parameters()
{
    delete ui;
}

void CWE_Parameters::linkWithDriver(VWTinterfaceDriver * newDriver)
{
    myDriver = newDriver;
    QObject::connect(myDriver, SIGNAL(haveNewCase()),
                     this, SLOT(newCaseGiven()));
    ui->theTabWidget->setupDriver(myDriver);
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
    resetViewInfo();
}

void CWE_Parameters::newCaseState(CaseState newState)
{
    //TODO: implement functions for changes in current params or stage states
}

void CWE_Parameters::switchToResultsSlot()
{
    emit switchToResultsTab();
}

void CWE_Parameters::switchToParameterSlot()
{
    emit switchToParameterTab();
}

void CWE_Parameters::switchToCreateSlot()
{
    emit switchToCreateTab();
}
