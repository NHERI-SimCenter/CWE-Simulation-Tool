#include "cwe_simulation_details.h"
#include "ui_cwe_simulation_details.h"

CWE_simulation_details::CWE_simulation_details(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_simulation_details)
{
    ui->setupUi(this);
}

CWE_simulation_details::~CWE_simulation_details()
{
    delete ui;
}
