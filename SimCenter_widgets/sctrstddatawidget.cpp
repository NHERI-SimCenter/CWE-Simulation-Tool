#include "sctrstddatawidget.h"

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
    theInputWidget = new QLineEdit(this);
    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->insertWidget(1, theInputWidget, 4);

    this->setLayout(layout);  // do I need this one?
}

QString SCtrStdDataWidget::toString()
{
    return ((QLineEdit *)theInputWidget)->text();
}

double SCtrStdDataWidget::toDouble()
{
    return ((QLineEdit *)theInputWidget)->text().toDouble();
}
