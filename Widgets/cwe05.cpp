#include "cwe05.h"
#include "ui_cwe05.h"

CWE05::CWE05(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE05)
{
    ui->setupUi(this);
}

CWE05::~CWE05()
{
    delete ui;
}
