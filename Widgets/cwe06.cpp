#include "cwe06.h"
#include "ui_cwe06.h"

CWE06::CWE06(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE06)
{
    ui->setupUi(this);
}

CWE06::~CWE06()
{
    delete ui;
}
