#include "cwe03.h"
#include "ui_cwe03.h"

CWE03::CWE03(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE03)
{
    ui->setupUi(this);
}

CWE03::~CWE03()
{
    delete ui;
}
