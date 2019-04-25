#include "inflowparameterwidget.h"
#include "ui_inflowparameterwidget.h"

#include "math.h"

InflowParameterWidget::InflowParameterWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::InflowParameterWidget)
{
    ui->setupUi(this);
}

InflowParameterWidget::~InflowParameterWidget()
{
    delete ui;
}

void InflowParameterWidget::on_btnNormalize_clicked()
{
    double z1 = ui->zVector1->value();
    double z2 = ui->zVector2->value();
    double z3 = ui->zVector3->value();

    double norm = sqrt(z1*z1 + z2*z2 + z3+z3);

    if (norm > 1.0e-3)
    {
        ui->zVector1->setValue(z1/norm);
        ui->zVector2->setValue(z2/norm);
        ui->zVector3->setValue(z3/norm);
    }
}

void InflowParameterWidget::on_PHI21_valueChanged(double arg1)
{
    ui->PHI12->setValue(arg1);
}

void InflowParameterWidget::on_PHI31_valueChanged(double arg1)
{
    ui->PHI13->setValue(arg1);
}

void InflowParameterWidget::on_PHI32_valueChanged(double arg1)
{
    ui->PHI23->setValue(arg1);
}
