#include "cwe01.h"
#include "ui_cwe01.h"

CWE01::CWE01(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE01)
{
    ui->setupUi(this);
}

CWE01::~CWE01()
{
    delete ui;
}
