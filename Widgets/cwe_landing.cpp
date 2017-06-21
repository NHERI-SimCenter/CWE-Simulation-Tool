#include "cwe_landing.h"
#include "ui_cwe_landing.h"

CWE_landing::CWE_landing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_landing)
{
    ui->setupUi(this);
}

CWE_landing::~CWE_landing()
{
    delete ui;
}
