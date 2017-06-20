#include "cwe07.h"
#include "ui_cwe07.h"

CWE07::CWE07(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE07)
{
    ui->setupUi(this);
}

CWE07::~CWE07()
{
    delete ui;
}
