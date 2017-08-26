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
        if (name == "mesh") {
            //ui->theTabWidget->setTabText(0,"Mesh Parameters");
            QTabWidget * master = new QTabWidget();
            meshIdx = ui->theTabWidget->addMasterTab(master, "Mesh\nParameters");
        }
        else if (name == "sim") {
            //ui->theTabWidget->setTabText(1,"Simulation Parameters");
            QTabWidget * master = new QTabWidget();
            simuIdx = ui->theTabWidget->addMasterTab(master, "Simulation\nParameters");
        }
        else if (name == "post") {
            QTabWidget * master = new QTabWidget();
            postIdx = ui->theTabWidget->addMasterTab(master, "Post\nProcessing\nParameters");
        }
        else {
            QTabWidget * master = new QTabWidget();
            ui->theTabWidget->addMasterTab(master, name);
        }
    }

    QJsonArray    meshStages = stages["mesh"].toArray();
    QJsonArray    simulationStages = stages["sim"].toArray();
    QJsonObject   varGroups  = obj["varGroups"].toObject();
    QJsonObject   vars       = obj["vars"].toObject();
    //QJsonArray    results    = obj["results"].toArray();

    this->setType(obj["name"].toString());

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

            QString vKey = meshParam.toString();
            QJsonObject vObj = vars[vKey].toObject();
            QString displayname = vObj["displayname"].toString();
            QString type        = vObj["type"].toString();

            if (type == "std") {
                QVariant defaultOption = vObj["default"].toVariant();
                QString unit           = vObj["unit"].toString();
                // double def = defaultOption.toDouble();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QDoubleSpinBox *theValue = new QDoubleSpinBox(displayWidget);
                theValue->setValue(defaultOption.toDouble());
                QLabel *theUnit = new QLabel(displayWidget);
                theUnit->setText(unit);
                int row = layout->rowCount();
                layout->addWidget(theName, row,0);
                layout->addWidget(theValue,row,1);
                layout->addWidget(theUnit, row,2);
            }
            else if (type == "choose") {
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QComboBox *theSelection = new QComboBox(displayWidget);
                QJsonObject combo_options = vObj["options"].toObject();
                QStandardItemModel *theModel = new QStandardItemModel();
                foreach (const QString &theKey, combo_options.keys())
                {
                    //QStandardItem *itm = new QStandardItem(theKey);
                    QStandardItem *itm = new QStandardItem(combo_options[theKey].toString());
                    theModel->appendRow(itm);
                }
                theSelection->setModel(theModel);
                theSelection->setCurrentText(combo_options[vObj["default"].toString()].toString());
                int row = layout->rowCount();
                layout->addWidget(theName, row,0);
                layout->addWidget(theSelection,row,1,1,2);
            }
            else if (type == "file") {
                // a filename
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QLineEdit *theFileName = new QLineEdit(displayWidget);
                theFileName->setText("unknown file name");
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
                layout->addWidget(theFileName,row,1,1,2);
            }
            else if (type == "bool") {
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QCheckBox *theBox = new QCheckBox(displayWidget);
                theBox->setChecked(vObj["default"].toBool());
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
                layout->addWidget(theBox, row,1);
            }
            else {
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
            }
        }
        layout->addItem(new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount(), 2);
    }

    QMap<QString, QWidget *> simulationTabWidgets;

    foreach (const QJsonValue & simulationTab, simulationStages)
    {
        CWE_ParameterTab *simTab = new CWE_ParameterTab();
        QString key = simulationTab.toString();
        simulationTabWidgets.insert(key, simTab);

        //ui->simulation_tabWidget->addTab(simTab, key);
        ui->theTabWidget->addTab(simTab, key);

        QWidget *displayWidget = simTab->getParameterSpace();

        QGridLayout *layout = (QGridLayout*)(displayWidget->layout());

        QJsonArray paramList = varGroups[key].toArray();

        foreach (const QJsonValue & meshParam, paramList)
        {
            nParameters++;

            QString vKey = meshParam.toString();
            QJsonObject vObj = vars[vKey].toObject();
            QString displayname = vObj["displayname"].toString();
            QString type        = vObj["type"].toString();

            if (type == "std") {
                QVariant defaultOption = vObj["default"].toVariant();
                QString unit           = vObj["unit"].toString();
                // double def = defaultOption.toDouble();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QDoubleSpinBox *theValue = new QDoubleSpinBox(displayWidget);
                theValue->setValue(defaultOption.toDouble());
                QLabel *theUnit = new QLabel(displayWidget);
                theUnit->setText(unit);
                int row = layout->rowCount();
                layout->addWidget(theName, row,0);
                layout->addWidget(theValue,row,1);
                layout->addWidget(theUnit, row,2);
            }
            else if (type == "choose") {
                QJsonArray options     = vObj["options"].toArray();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QComboBox *theSelection = new QComboBox(displayWidget);
                QJsonObject combo_options = vObj["options"].toObject();
                QStandardItemModel *theModel = new QStandardItemModel();
                foreach (const QString &theKey, combo_options.keys())
                {
                    //QStandardItem *itm = new QStandardItem(theKey);
                    QStandardItem *itm = new QStandardItem(combo_options[theKey].toString());
                    theModel->appendRow(itm);
                }
                theSelection->setModel(theModel);
                theSelection->setCurrentText(combo_options[vObj["default"].toString()].toString());
                int row = layout->rowCount();
                layout->addWidget(theName, row,0);
                layout->addWidget(theSelection,row,1,1,2);
            }
            else if (type == "file") {
                // a filename

            }
            else if (type == "bool") {
                QVariant defaultOption = vObj["default"].toVariant();
                // bool def = defaultOption.toBool();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QCheckBox *theBox = new QCheckBox(displayWidget);
                theBox->setChecked(vObj["default"].toBool());
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
                layout->addWidget(theBox, row,1);
            }
            else {
                QVariant defaultOption = vObj["default"].toVariant();
                // bool def = defaultOption.toBool();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
            }
        }
        layout->addItem(new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount(), 2);
    }

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

