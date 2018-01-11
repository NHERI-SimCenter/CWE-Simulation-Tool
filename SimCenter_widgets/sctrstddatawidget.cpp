#include "SimCenter_widgets/sctrstddatawidget.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

#include <QHBoxLayout>

#include "qdebug.h"

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
    m_obj = obj;

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
    this->updateValue(defaultValue);
}

QString SCtrStdDataWidget::toString()
{
    QString s = "";

    QString precString  = m_obj.value("precision").toString();
    if (precString.toLower() == "int")
    {
        s = QString("%1").arg(((theValue->text()).toInt()));
    }
    else
    {
        int prec = precString.toInt();
        s = QString("%1").arg(((theValue->text()).toDouble()), 0, 'f', prec);
    }
    return s;
}

double SCtrStdDataWidget::toDouble()
{
    return theValue->text().toDouble();
}

void SCtrStdDataWidget::updateValue(QString s)
{
    QString val = "";

    /* check if new information is of an appropriate type */

    QString precString  = m_obj.value("precision").toString();
    if (precString.toLower() == "int")
    {
        val = QString("%1").arg(s.toInt());
    }
    else
    {
        int prec = precString.toInt();
        val = QString("%1").arg(s.toDouble(), 0, 'f', prec);
    }

    /* update the value */
    theValue->setText(val);
}
