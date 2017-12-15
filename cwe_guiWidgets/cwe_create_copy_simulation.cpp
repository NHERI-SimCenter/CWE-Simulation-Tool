#include "cwe_create_copy_simulation.h"
#include "ui_cwe_create_copy_simulation.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/remotefiletree.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "vwtinterfacedriver.h"

CWE_Create_Copy_Simulation::CWE_Create_Copy_Simulation(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Create_Copy_Simulation)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);
}

CWE_Create_Copy_Simulation::~CWE_Create_Copy_Simulation()
{
    delete ui;
}

void CWE_Create_Copy_Simulation::linkDriver(VWTinterfaceDriver * theDriver)
{
    CWE_Super::linkDriver(theDriver);
    this->populateCaseTypes();
    if (!theDriver->inOfflineMode())
    {
        ui->primary_remoteFileTree->setFileOperator(theDriver->getFileHandler());
        ui->primary_remoteFileTree->setupFileView();
        ui->secondary_remoteFileTree->setFileOperator(theDriver->getFileHandler());
        ui->secondary_remoteFileTree->setupFileView();
    }
}

void CWE_Create_Copy_Simulation::on_pBtn_create_copy_clicked()
{
    if (myDriver->inOfflineMode())
    {
        myDriver->setCurrentCase(new CFDcaseInstance(selectedTemplate, myDriver));
        myDriver->getMainWindow()->switchToParameterTab();
        return;
    }

    /* take emergency exit if nothing has been selected */
    FileTreeNode * selectedNode = ui->primary_remoteFileTree->getSelectedNode();
    if (selectedNode == NULL) { return; }

    /* OK, something has been selected */
    CFDcaseInstance * newCase;

    //TODO: VERY IMPORTANT: NEED INPUT FILTERING
    // PETER S.: what have you been thinkng of with this statement?

    if (ui->tabWidget->currentWidget() == ui->tab_NewCase)
    {
        /* we are creating a new case */

        if (selectedTemplate == NULL) return;
        newCase = new CFDcaseInstance(selectedTemplate, myDriver);
        newCase->createCase(ui->lineEdit_newCaseName->text(), selectedNode);
    }
    else
    {
        /* we are cloning from an existing case */

        FileTreeNode * secondNode = ui->secondary_remoteFileTree->getSelectedNode();
        if (secondNode == NULL)
        {
            return;
        }
        newCase = new CFDcaseInstance(myDriver);
        newCase->duplicateCase(ui->lineEdit_newCaseName->text(), selectedNode, secondNode);
        if (newCase->getCaseState() != CaseState::OP_INVOKE)
        {
            return;
        }
    }

    //Set new case will signal the other panels so that they can get configurations
    myDriver->setCurrentCase(newCase);

    /* time to switch to the ParameterTab */
    myDriver->getMainWindow()->switchToParameterTab();
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

void CWE_Create_Copy_Simulation::populateCaseTypes()
{
    QList<CFDanalysisType *> * templateList = myDriver->getTemplateList();
    QGridLayout *layout = new QGridLayout();

    int idx = 0;

    foreach (CFDanalysisType * caseType, *templateList) {
        /* get analysis type and name info from JSON */
        QJsonDocument * configuration = caseType->getRawConfig();

        QJsonObject confObj = configuration->object();
        QString theName = confObj["name"].toString();
        QString theDescription = confObj["description"].toString();

        /* create UI selection block */
        QRadioButton *radioBtn = new QRadioButton(theName, ui->scroll_NewCase);
        QPushButton *buttonIcon = new QPushButton(ui->scroll_NewCase);

        buttonIcon->setIcon(*(caseType->getIcon()));
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
        list.templateData = caseType;
        caseTypeDataList.append(list);

        /* create appropriate connection between signals and slots */

        QObject::connect(buttonIcon, SIGNAL(pressed()),       this, SLOT(selectCaseTemplate()));
        QObject::connect(radioBtn, SIGNAL(toggled(bool)),     this, SLOT(selectCaseTemplate()));

        idx++;
    }

    QLayout *lyt = ui->scroll_NewCase->layout();
    if (lyt != NULL) {delete lyt;}

    ui->scroll_NewCase->setLayout(layout);
}

void CWE_Create_Copy_Simulation::selectCaseTemplate()
{
    QObject *sender = QObject::sender();
    QVector<CASE_TYPE_DATA>::iterator i;

    for (i = caseTypeDataList.begin(); i != caseTypeDataList.end(); i++) {
        if  (i->pbtn == (QPushButton *)sender) {
            i->radioBtn->setChecked(true);

            selectedTemplate = i->templateData;

            break;
        }
        else if  (i->radioBtn == (void *)sender ) {
            /* QRadioButton toggled -- Qt is taking care of this */
            //i->radioBtn->setChecked(true);

            /* initiate case */
            selectedTemplate = i->templateData;

            break;
        }
        else {
            /* turn off not selected QRadioButton -- Qt is taking care of this */
            //((QRadioButton *)(*i)[0])->setChecked(false);
        }
    }
}
