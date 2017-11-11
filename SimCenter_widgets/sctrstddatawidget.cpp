#include "sctrstddatawidget.h"

#include <QHBoxLayout>

SCtrStdDataWidget::SCtrStdDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

void SCtrStdDataWidget::setData(QJsonObject &obj)
{
    theInputWidget = new QLineEdit(this);
    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->insertWidget(1, theInputWidget, 4);

    this->setLayout(layout);  // do I need this one?
}
