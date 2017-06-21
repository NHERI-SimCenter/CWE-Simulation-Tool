#include "cwe_help.h"
#include "ui_cwe_help.h"

CWE_help::CWE_help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_help)
{
    ui->setupUi(this);
}

CWE_help::~CWE_help()
{
    delete ui;
}
