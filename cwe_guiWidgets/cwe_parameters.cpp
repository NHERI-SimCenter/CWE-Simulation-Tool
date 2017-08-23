#include "cwe_parameters.h"
#include "ui_cwe_parameters.h"

CWE_Parameters::CWE_Parameters(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Parameters)
{
    ui->setupUi(this);
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
