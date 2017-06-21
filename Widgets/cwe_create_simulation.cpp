#include "cwe_create_simulation.h"
#include "ui_cwe_create_simulation.h"

CWE_create_simulation::CWE_create_simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_create_simulation)
{
    ui->setupUi(this);
}

CWE_create_simulation::~CWE_create_simulation()
{
    delete ui;
}
