#include "cwe04a.h"
#include "ui_cwe04a.h"

CWE04a::CWE04a(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE04a)
{
    ui->setupUi(this);
}

CWE04a::~CWE04a()
{
    delete ui;
}
