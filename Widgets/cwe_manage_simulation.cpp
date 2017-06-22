#include "cwe_manage_simulation.h"
#include "ui_cwe_manage_simulation.h"

CWE_manage_simulation::CWE_manage_simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_manage_simulation)
{
    ui->setupUi(this);
}

CWE_manage_simulation::~CWE_manage_simulation()
{
    delete ui;
}
