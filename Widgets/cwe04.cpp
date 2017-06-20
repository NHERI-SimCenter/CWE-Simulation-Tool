#include "cwe04.h"
#include "ui_cwe04.h"

CWE04::CWE04(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE04)
{
    ui->setupUi(this);
}

CWE04::~CWE04()
{
    delete ui;
}
