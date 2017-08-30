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

#include <CFDanalysis/CFDanalysisType.h>

CWE_Parameters::CWE_Parameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Parameters)
{
    ui->setupUi(this);
    meshIdx = -1;
    simuIdx = -1;
    postIdx = -1;
}

CWE_Parameters::~CWE_Parameters()
{
    delete ui;
}

void CWE_Parameters::setName(const QString &s)     {ui->label_theName->setText(s);}
void CWE_Parameters::setType(const QString &s)     {ui->label_theType->setText(s);}
void CWE_Parameters::setLocation(const QString &s) {ui->label_theLocation->setText(s);}

int CWE_Parameters::setTemplate(CFDanalysisType * theTemplate)
{
    int nParameters = 0;

    QJsonObject   obj    = theTemplate->getRawConfig()->object();

    this->setType(obj["name"].toString());

    QJsonObject    stages     = obj["stages"].toObject();
    QList<QString> stageNames = stages.keys();
    QJsonObject    varGroups  = obj["varGroups"].toObject();
    QJsonObject    vars       = obj["vars"].toObject();

    //QJsonArray    results    = obj["results"].toArray();

    //qDebug() << stageNames;

    int cnt = 0;

    foreach (QString name, stageNames)
    {
        QString labelText;

        if      (name == "mesh") { labelText = "Mesh\nParameters"; }
        else if (name == "sim" ) { labelText = "Simulation\nParameters"; }
        else if (name == "post") { labelText = "Post\nProcessing\nParameters"; }
        else                     { labelText = name; }

        // add a stage tab to ui->theTabWidget
        int idx = ui->theTabWidget->addGroupTab(name, labelText);
        parameterTabs.insert(name, idx);

        // add varGroub tabs
        QJsonArray theGroups = stages[name].toArray();
        foreach (const QJsonValue item, theGroups)
        {
            QString subTitle = item.toString();
            QJsonArray varList = varGroups[subTitle].toArray();
            ui->theTabWidget->addVarTab(name, subTitle, &varList, &vars);
        }

        cnt++;
    }

    if (cnt>0) {ui->theTabWidget->setIndex(0);}

    return nParameters;
}

/*
void CWE_Parameters::on_pBtn_simulation_run_clicked()
{

}

void CWE_Parameters::on_pBtn_simulation_cancel_clicked()
{

}

void CWE_Parameters::on_pBtn_simulation_results_clicked()
{

}

void CWE_Parameters::on_pBtn_simulation_rollback_clicked()
{

}

void CWE_Parameters::on_pBtn_model_run_clicked()
{

}

void CWE_Parameters::on_pBtn_model_cancel_clicked()
{

}

void CWE_Parameters::on_pBtn_model_results_clicked()
{

}

void CWE_Parameters::on_pBtn_model_rollback_clicked()
{

}

void CWE_Parameters::on_pbtn_saveAllParameters_clicked()
{

}
*/

