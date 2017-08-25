#include "cwe_parametertab.h"
#include "ui_cwe_parametertab.h"

CWE_ParameterTab::CWE_ParameterTab(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::CWE_ParameterTab)
{
    ui->setupUi(this);
}

CWE_ParameterTab::~CWE_ParameterTab()
{
    delete ui;
}

QWidget * CWE_ParameterTab::getParameterSpace()
{
    return ui->parameterSpace;
}
