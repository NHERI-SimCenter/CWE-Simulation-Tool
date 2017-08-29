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

    QJsonObject    stages     = obj["stages"].toObject();
    QList<QString> stageNames = stages.keys();

    //qDebug() << stageNames;

    foreach (QString name, stageNames)
    {
        QString labelText;

        if      (name == "mesh") { labelText = "Mesh\nParameters"; }
        else if (name == "sim" ) { labelText = "Simulation\nParameters"; }
        else if (name == "post") { labelText = "Post\nProcessing\nParameters"; }
        else                     { labelText = name; }

        int idx = ui->theTabWidget->addGroupTab(name, labelText);
        parameterTabs.insert(name, idx);
    }

    QJsonArray    meshStages = stages["mesh"].toArray();
    QJsonArray    simulationStages = stages["sim"].toArray();
    QJsonObject   varGroups  = obj["varGroups"].toObject();
    QJsonObject   vars       = obj["vars"].toObject();
    //QJsonArray    results    = obj["results"].toArray();

    this->setType(obj["name"].toString());

#if 0
    QMap<QString, QWidget *> meshTabWidgets;

    foreach (const QJsonValue & meshTab, meshStages)
    {
        CWE_ParameterTab *paramTab = new CWE_ParameterTab();
        QString key = meshTab.toString();
        meshTabWidgets.insert(key, paramTab);

        //ui->mesh_tabWidget->addTab(paramTab, key);
        ui->theTabWidget->addTab(paramTab, key);

        QWidget *displayWidget = paramTab->getParameterSpace();

        QGridLayout *layout = (QGridLayout*)(displayWidget->layout());

        QJsonArray paramList = varGroups[key].toArray();

        foreach (const QJsonValue & meshParam, paramList)
        {
            nParameters++;

            QString vKey     = meshParam.toString();
            QJsonObject vObj = vars[vKey].toObject();
            QString type     = vObj["type"].toString();

            ui->theTabWidget->addType(type, vObj);
        }
        layout->addItem(new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount(), 2);
    }
#endif

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

