#include "cwe_create_copy_simulation.h"
#include "ui_cwe_create_copy_simulation.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/remotefiletree.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "vwtinterfacedriver.h"

CWE_Create_Copy_Simulation::CWE_Create_Copy_Simulation(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_Create_Copy_Simulation)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentIndex(0);
    this->setSimulationType(SimulationType::CHANNEL_FLOW);
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

    ui->radioButton_channelFlow->setChecked(false);
    ui->radioButton_2Dshape->setChecked(false);
    ui->radioButton_3Dshape->setChecked(false);

    switch (type) {
    case SimulationType::CHANNEL_FLOW:
        ui->radioButton_channelFlow->setChecked(true);
        break;
    case SimulationType::SHAPE_2D:
        ui->radioButton_2Dshape->setChecked(true);
        break;
    case SimulationType::SHAPE_3D:
        ui->radioButton_3Dshape->setChecked(true);
        break;
    default:
        ui->radioButton_channelFlow->setChecked(true);
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
