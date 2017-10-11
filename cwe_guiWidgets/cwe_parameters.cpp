#include "cwe_parameters.h"
#include "ui_cwe_parameters.h"
#include "cwe_parametertab.h"

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
}

void CWE_Parameters::resetViewInfo()
{
    viewIsValid = false;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL) return;
    CFDanalysisType * theTemplate = currentCase->getMyType();
    if (theTemplate == NULL) return;
    QMap<QString, StageState> currentStates = currentCase->getStageStates();

    viewIsValid = true;

    ui->label_theName->setText(currentCase->getCaseName());
    ui->label_theType->setText(theTemplate->getName());
    ui->label_theLocation->setText(currentCase->getCaseFolder());

    QJsonObject   obj    = theTemplate->getRawConfig()->object();

    QJsonObject    stages     = obj["stages"].toObject();
    QList<QString> stageNames = stages.keys();
    QJsonObject    varGroups  = obj["varGroups"].toObject();
    QJsonObject    vars       = obj["vars"].toObject();

    QMap<QString,QString> setVars = currentCase->getCurrentParams();

    //QJsonArray    results    = obj["results"].toArray();

    //qDebug() << stageNames;

    int cnt = 0;

    foreach (QString name, stageNames)
    {
        QJsonObject stageInfo = stages[name].toObject();
        QString labelText;

        labelText = stageInfo["name"].toString();
        labelText = labelText.append("\nParameters");

        // add a stage tab to ui->theTabWidget
        int idx = ui->theTabWidget->addGroupTab(name, labelText, currentStates[name]);
        parameterTabs.insert(name, idx);

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

void CWE_Parameters::newCaseState(CaseState oldState, CaseState newState)
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
