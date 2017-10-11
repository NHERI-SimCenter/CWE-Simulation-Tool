#include "cwe_create_copy_simulation.h"
#include "ui_cwe_create_copy_simulation.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/remotefiletree.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QRadioButton>
#include <QLabel>

CWE_Create_Copy_Simulation::CWE_Create_Copy_Simulation(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_Create_Copy_Simulation)
{
    ui->setupUi(this);

    /* populate tab_NewCase with available cases */
    QDir confDir(":/config");
    QStringList filters;
    filters << "*.json" << "*.JSON";
    QStringList caseTypeFiles = confDir.entryList(filters);

    this->populateCaseTypes(caseTypeFiles);

    ui->tabWidget->setCurrentIndex(0);
    this->setSimulationType(SimulationType::CHANNEL_FLOW);
}

CWE_Create_Copy_Simulation::~CWE_Create_Copy_Simulation()
{
    delete ui;
}

void CWE_Create_Copy_Simulation::linkFileHandle(FileOperator * theJobhandle)
{
    ui->primary_remoteFileTree->setFileOperator(theJobhandle);
    ui->primary_remoteFileTree->setupFileView();
    ui->secondary_remoteFileTree->setFileOperator(theJobhandle);
    ui->secondary_remoteFileTree->setupFileView();
}

void CWE_Create_Copy_Simulation::on_lineEdit_newCaseName_editingFinished()
{

}

void CWE_Create_Copy_Simulation::on_pBtn_cancel_clicked()
{

}

void CWE_Create_Copy_Simulation::on_pBtn_create_copy_clicked()
{

}

void CWE_Create_Copy_Simulation::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0: // create new case
        ui->pBtn_create_copy->setText("Create New Simulation");
        break;
    case 1: // duplicate an existing case
        ui->pBtn_create_copy->setText(tr("Duplicate && Edit"));
        break;
    default:
        // this one should not happen.
        ui->tabWidget->setCurrentIndex(0);
    }
}

void CWE_Create_Copy_Simulation::on_radioButton_2Dshape_clicked()
{
    this->setSimulationType(SimulationType::SHAPE_2D);
}

void CWE_Create_Copy_Simulation::on_radioButton_3Dshape_clicked()
{
    this->setSimulationType(SimulationType::SHAPE_3D);
}

void CWE_Create_Copy_Simulation::on_radioButton_channelFlow_clicked()
{
    this->setSimulationType(SimulationType::CHANNEL_FLOW);
}

SimulationType CWE_Create_Copy_Simulation::setSimulationType(SimulationType type)
{
    SimulationType retType = type;

    //ui->radioButton_channelFlow->setChecked(false);
    //ui->radioButton_2Dshape->setChecked(false);
    //ui->radioButton_3Dshape->setChecked(false);

    switch (type) {
    case SimulationType::CHANNEL_FLOW:
      //  ui->radioButton_channelFlow->setChecked(true);
        break;
    case SimulationType::SHAPE_2D:
      //  ui->radioButton_2Dshape->setChecked(true);
        break;
    case SimulationType::SHAPE_3D:
      //  ui->radioButton_3Dshape->setChecked(true);
        break;
    default:
      //  ui->radioButton_channelFlow->setChecked(true);
        retType = SimulationType::CHANNEL_FLOW;
    }

    return retType;
}

void CWE_Create_Copy_Simulation::on_pb_image_channelFlow_clicked()
{
    this->setSimulationType(SimulationType::CHANNEL_FLOW);
}

void CWE_Create_Copy_Simulation::on_pb_image_2Dshape_clicked()
{
    this->setSimulationType(SimulationType::SHAPE_2D);
}

void CWE_Create_Copy_Simulation::on_pb_image_3Dshape_clicked()
{
    this->setSimulationType(SimulationType::SHAPE_3D);
}

void CWE_Create_Copy_Simulation::populateCaseTypes(QStringList &caseTypeFiles)
{
    QGridLayout *layout = new QGridLayout(this);

    foreach (QString caseType, caseTypeFiles) {
        /* read JSON info from file */
        QString configFile = ":/config/" + caseType;

        QFile inFile(configFile);
        if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) continue;
        QByteArray val = inFile.readAll();
        inFile.close();

        /* get analysis type and name info from JSON */
        QJsonDocument configuration = QJsonDocument::fromJson(val);

        QJsonObject confObj = configuration.object();
        QString theName = confObj["name"].toString();
        QString theDescription = confObj["description"].toString();
        QString theIcon = confObj["icon"].toString();
        QString theIconPath = ":/config/" + theIcon;

        /* create UI selection block */
        QRadioButton *radioBtn = new QRadioButton(theName, ui->scroll_NewCase);
        QLabel *labelIcon = new QLabel(ui->scroll_NewCase);
        if (theIcon == "") {
            labelIcon->setPixmap(QPixmap(":/buttons/images/defaultCaseImage.png"));
        }
        else {
            labelIcon->setPixmap(QPixmap(theIconPath));
        }
        labelIcon->setMinimumSize(150, 100);
        labelIcon->setMaximumSize(150, 100);
        QLabel *labelDescription = new QLabel(ui->scroll_NewCase);
        if (theDescription == "") {
            theDescription = "some\ndescription\nof this\ncase.";
        }
        labelDescription->setText(theDescription);

        int cnt = layout->rowCount();
        layout->addWidget(labelIcon,cnt+1,1,1,1);
        layout->addWidget(labelDescription,cnt+1,2,1,1);
        layout->addWidget(radioBtn,cnt,1,1,2);

        /* create appropriate connection between signals and slots */

        connect(labelIcon, SIGNAL(clicked()),        this, SLOT(selectCaseTemplate()));
        connect(labelDescription, SIGNAL(clicked()), this, SLOT(selectCaseTemplate()));
        connect(radioBtn, SIGNAL(toggled(bool)),     this, SLOT(selectCaseTemplate()));

    }

    ui->scroll_NewCase->setLayout(layout);
}

void CWE_Create_Copy_Simulation::selectCaseTemplate()
{
    QObject *sender = QObject::sender();
}
