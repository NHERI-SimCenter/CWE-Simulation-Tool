#include "sctrfiledatawidget.h"

#include <QHBoxLayout>
#include <QLineEdit>

SCtrFileDataWidget::SCtrFileDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrFileDataWidget::SCtrFileDataWidget(QJsonObject &obj, QWidget *parent):
    SCtrMasterDataWidget(parent)
{
    this->setData(obj);
}

void SCtrFileDataWidget::setData(QJsonObject &obj)
{
    m_obj = obj;

    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    theValue = new QLineEdit(this);
    layout->insertWidget(1, theCheckBox, 4);

    if (label_unit != NULL) {
        label_unit->setText(obj.value(QString("unit")).toString());
    }
    if (label_varName != NULL) {
        label_varName->setText(obj.value(QString("displayname")).toString());
    }

    this->setLayout(layout);  // do I need this one?

    /* set default */
    QString defaultValue = obj.value(QString("default")).toString();
    theValue->setText(defaultValue);
}

bool SCtrFileDataWidget::toBool()
{
    QString text = theValue->text();
    return !text.isEmpty();
}

QString SCtrFileDataWidget::toString()
{
    return theValue->text();
}

void SCtrFileDataWidget::updateValue(QString s)
{
    /* update the value */
    theValue->setText(s);
}

