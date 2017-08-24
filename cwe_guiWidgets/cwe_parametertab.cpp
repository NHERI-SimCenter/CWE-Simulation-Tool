#include "cwe_parametertab.h"
#include "ui_cwe_parametertab.h"

CWE_ParameterTab::CWE_ParameterTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_ParameterTab)
{
    ui->setupUi(this);
}

CWE_ParameterTab::~CWE_ParameterTab()
{
    delete ui;
}
