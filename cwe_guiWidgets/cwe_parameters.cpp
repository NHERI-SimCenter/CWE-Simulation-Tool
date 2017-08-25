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

CWE_Parameters::CWE_Parameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Parameters)
{
    ui->setupUi(this);

    this->setTemplate(":/config/building2D.json");
}

CWE_Parameters::~CWE_Parameters()
{
    delete ui;
}

void CWE_Parameters::setName(const QString &s)     {ui->label_theName->setText(s);}
void CWE_Parameters::setType(const QString &s)     {ui->label_theType->setText(s);}
void CWE_Parameters::setLocation(const QString &s) {ui->label_theLocation->setText(s);}



void CWE_Parameters::on_pbtn_saveAllParameters_clicked()
{
    /* save all parameters */

}

int CWE_Parameters::setTemplate(const QString &filename)
{
    int nParameters = 0;

    QString val;
    QFile file;

    QStringList propertyNames;
    QStringList propertyKeys;

    file.setFileName(filename);
    //file.setFileName(":/config/building2D.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument config = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject   obj    = config.object();

    QJsonObject   stages     = obj["stages"].toObject();
    QJsonArray    meshStages = stages["mesh"].toArray();
    QJsonArray    simulationStages = stages["sim"].toArray();
    QJsonObject   varGroups  = obj["varGroups"].toObject();
    QJsonObject   vars       = obj["vars"].toObject();
    QJsonArray    results    = obj["results"].toArray();

    this->setType(obj["name"].toString());

    QMap<QString, QWidget *> meshTabWidgets;

    foreach (const QJsonValue & meshTab, meshStages)
    {
        QWidget *paramTab = new CWE_ParameterTab();
        QString key = meshTab.toString();
        meshTabWidgets.insert(key, paramTab);
        ui->mesh_tabWidget->addTab(paramTab, key);

        QWidget *displayWidget = paramTab->findChild<QWidget *>("parameterSpace");

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
                QVariant defaultOption = vObj["default"].toVariant();
                // bool def = defaultOption.toBool();
                QLabel *theName = new QLabel(displayWidget);
                theName->setText(displayname);
                QLineEdit *theFileName = new QLineEdit(displayWidget);
                theFileName->setText("unknown file name");
                int row = layout->rowCount();
                layout->addWidget(theName,row,0);
                layout->addWidget(theFileName,row,1,1,2);
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

    QMap<QString, QWidget *> simulationTabWidgets;

    foreach (const QJsonValue & simulationTab, simulationStages)
    {
        QWidget *simTab = new CWE_ParameterTab();
        QString key = simulationTab.toString();
        simulationTabWidgets.insert(key, simTab);
        ui->simulation_tabWidget->addTab(simTab, key);

        QWidget *displayWidget = simTab->findChild<QWidget *>("parameterSpace");

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

    // this is crazy slow -- requires a better solution
    QFile styleFile(":/cweStyle.qss");
    QString styleText(styleFile.readAll());
    QList<QWidget *> container = this->findChildren<QWidget *>();
    foreach ( QWidget *var, container) {
        var->setStyleSheet(styleText);
    }

    return nParameters;
}

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
