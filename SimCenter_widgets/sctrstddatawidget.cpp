#include "SimCenter_widgets/sctrstddatawidget.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

#include <QHBoxLayout>

SCtrStdDataWidget::SCtrStdDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrStdDataWidget::SCtrStdDataWidget(QJsonObject &obj, QWidget *parent):
    SCtrMasterDataWidget(parent)
{
    this->setData(obj);
}

void SCtrStdDataWidget::setData(QJsonObject &obj)
{
    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    theValue = new QLineEdit(this);
    layout->insertWidget(1, theValue, 4);

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

QString SCtrStdDataWidget::toString()
{
    return theValue->text();
}

double SCtrStdDataWidget::toDouble()
{
    return theValue->text().toDouble();
}
