#include "cwe_create_copy_simulation.h"
#include "ui_cwe_create_copy_simulation.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/remotefiletree.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "vwtinterfacedriver.h"

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
#include <QPushButton>
#include <QIcon>

CWE_Create_Copy_Simulation::CWE_Create_Copy_Simulation(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_Create_Copy_Simulation)
{
    ui->setupUi(this);

    //templateListMap = new QVector<QList<QWidget *> >;  // maps case idx to list of case widget*
    caseTypeDataList = new QVector<CASE_TYPE_DATA>;

    /* populate tab_NewCase with available cases */
    QDir confDir(":/config");
    QStringList filters;
    filters << "*.json" << "*.JSON";
    QStringList caseTypeFiles = confDir.entryList(filters);

    this->populateCaseTypes(caseTypeFiles);

    ui->tabWidget->setCurrentIndex(0);
    //this->setSimulationType(SimulationType::CHANNEL_FLOW);
}

CWE_Create_Copy_Simulation::~CWE_Create_Copy_Simulation()
{
    delete ui;
}

void CWE_Create_Copy_Simulation::linkDriver(VWTinterfaceDriver * theDriver)
{
    driverLink = theDriver;
    ui->primary_remoteFileTree->setFileOperator(theDriver->getFileHandler());
    ui->primary_remoteFileTree->setupFileView();
    ui->secondary_remoteFileTree->setFileOperator(theDriver->getFileHandler());
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
    //Note: some of this will be parsed to other methods
    //TODO: This is a first debug pass

    CFDanalysisType * debugType = driverLink->getTemplateList()->at(0);

    CFDcaseInstance * newCase = new CFDcaseInstance(debugType, driverLink);
    driverLink->setCurrentCase(newCase);

    //TODO: VERY IMPORTANT: NEED INPUT FILTERING
    newCase->createCase(ui->lineEdit_newCaseName->text(), ui->primary_remoteFileTree->getSelectedNode());

    emit needParamTab();
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

void CWE_Create_Copy_Simulation::populateCaseTypes(QStringList &caseTypeFiles)
{
    QGridLayout *layout = new QGridLayout(this);

    int idx = 0;

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

        /* create UI selection block */
        QRadioButton *radioBtn = new QRadioButton(theName, ui->scroll_NewCase);
        QPushButton *buttonIcon = new QPushButton(ui->scroll_NewCase);

        QString theIconPath;
        if (theIcon == "") {
            theIconPath = ":/buttons/images/defaultCaseImage.png";
        }
        else {
            theIconPath = ":/buttons/images/" + theIcon;
        }
        QIcon theBtnIcon = QIcon(theIconPath);
        buttonIcon->setIcon(theBtnIcon);
        buttonIcon->setIconSize(QSize(150,100));
        buttonIcon->setMinimumSize(150, 100);
        buttonIcon->setMaximumSize(150, 100);
        QLabel *labelDescription = new QLabel(ui->scroll_NewCase);
        if (theDescription == "") {
            theDescription = "some\ndescription\nof this\ncase.";
        }
        labelDescription->setText(theDescription);

        int cnt = layout->rowCount();
        layout->addWidget(buttonIcon,cnt+1,1,1,1);
        layout->addWidget(labelDescription,cnt+1,2,1,1);
        layout->addWidget(radioBtn,cnt,1,1,2);

        CASE_TYPE_DATA list;
        list.radioBtn = radioBtn;
        list.pbtn     = buttonIcon;
        list.caseFile = configFile;
        caseTypeDataList->append(list);


        /* create appropriate connection between signals and slots */

        connect(buttonIcon, SIGNAL(pressed()),       this, SLOT(selectCaseTemplate()));
        connect(radioBtn, SIGNAL(toggled(bool)),     this, SLOT(selectCaseTemplate()));

        idx++;
    }

    ui->scroll_NewCase->setLayout(layout);
}

void CWE_Create_Copy_Simulation::selectCaseTemplate()
{
    QObject *sender = QObject::sender();
    QVector<CASE_TYPE_DATA>::iterator i;

    for (i = caseTypeDataList->begin(); i != caseTypeDataList->end(); i++) {
        if  (i->pbtn == (QPushButton *)sender) {
            i->radioBtn->setChecked(true);

            /* initiate case */
            this->create_new_case_from_template(i->caseFile);

            break;
        }
        else if  (i->radioBtn == (void *)sender ) {
            /* QRadioButton toggled -- Qt is taking care of this */
            //i->radioBtn->setChecked(true);

            /* initiate case */
            this->create_new_case_from_template(i->caseFile);

            break;
        }
        else {
            /* turn off not selected QRadioButton -- Qt is taking care of this */
            //((QRadioButton *)(*i)[0])->setChecked(false);
        }
    }
}

void CWE_Create_Copy_Simulation::create_new_case_from_template(QString filename)
{
    /*
     * PETER S.:
     *
     * We just selected the analysis case type.
     * filename is the configuration file for the case.
     *
     * How do I properly initiate the driver/agave tool
     */

    /*

    //Note: some of this will be parsed to other methods
    //TODO: This is a first debug pass

    CFDanalysisType * debugType = driverLink->getTemplateList()->at(0);

    CFDcaseInstance * newCase = new CFDcaseInstance(debugType, driverLink);
    driverLink->setCurrentCase(newCase);

    //TODO: VERY IMPORTANT: NEED INPUT FILTERING
    newCase->createCase(ui->lineEdit_newCaseName->text(), ui->primary_remoteFileTree->getSelectedNode());

    */

    emit needParamTab();
}

