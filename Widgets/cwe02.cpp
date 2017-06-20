#include "cwe02.h"
#include "ui_cwe02.h"

CWE02::CWE02(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE02)
{
    ui->setupUi(this);
}

CWE02::~CWE02()
{
    delete ui;
}
