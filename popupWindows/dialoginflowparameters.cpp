#include "dialoginflowparameters.h"
#include "ui_dialoginflowparameters.h"

DialogInflowParameters::DialogInflowParameters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInflowParameters)
{
    ui->setupUi(this);
}

DialogInflowParameters::~DialogInflowParameters()
{
    delete ui;
}
