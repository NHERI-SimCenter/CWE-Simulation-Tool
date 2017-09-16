#include "cwe_busyindicator.h"
#include "ui_cwe_busyindicator.h"

CWE_BusyIndicator::CWE_BusyIndicator(QWidget *parent) :
    QFrame(parent, Qt::FramelessWindowHint),
    ui(new Ui::CWE_BusyIndicator)
{
    ui->setupUi(this);
}

CWE_BusyIndicator::~CWE_BusyIndicator()
{
    delete ui;
}
