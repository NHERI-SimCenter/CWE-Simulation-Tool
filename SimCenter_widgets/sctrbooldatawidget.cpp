#include "sctrbooldatawidget.h"

#include <QHBoxLayout>
#include <QCheckBox>

SCtrBoolDataWidget::SCtrBoolDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrBoolDataWidget::SCtrBoolDataWidget(QJsonObject &obj, QWidget *parent):
    SCtrMasterDataWidget(parent)
{
    this->setData(obj);
}

void SCtrBoolDataWidget::setData(QJsonObject &obj)
{
    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    theCheckBox = new QCheckBox(this);
    layout->insertWidget(1, theCheckBox, 4);

    if (label_unit != NULL) {
        label_unit->setText(obj.value(QString("unit")).toString());
    }
    if (label_varName != NULL) {
        label_varName->setText(obj.value(QString("displayname")).toString());
    }

    this->setLayout(layout);  // do I need this one?

    /* set default */
    bool defaultValue = obj.value(QString("default")).toBool();
    theCheckBox->setChecked(defaultValue?Qt::Checked:Qt::Unchecked);
}

bool SCtrBoolDataWidget::toBool()
{
    bool checked = theCheckBox->checkState()==Qt::Checked?true:false;
    return checked;
}

QString SCtrBoolDataWidget::toString()
{
    QString checked = theCheckBox->checkState()==Qt::Checked?QString("true"):QString("false");
    return checked;
}

double SCtrBoolDataWidget::toDouble()
{
    double checked = theCheckBox->checkState()==Qt::Checked?1.0:0.0;
    return checked;
}

void SCtrBoolDataWidget::setChecked()
{
    theCheckBox->setChecked(true);
}

void SCtrBoolDataWidget::setUnchecked()
{
    theCheckBox->setChecked(false);
}
