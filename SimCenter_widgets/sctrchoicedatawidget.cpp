#include "sctrchoicedatawidget.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>

#include <QDebug>

SCtrChoiceDataWidget::SCtrChoiceDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrChoiceDataWidget::SCtrChoiceDataWidget(QJsonObject &obj, QWidget *parent):
    SCtrMasterDataWidget(parent)
{
    this->setData(obj);
}

void SCtrChoiceDataWidget::setData(QJsonObject &obj)
{
    QHBoxLayout *layout = (QHBoxLayout *)this->layout();

    if (label_unit != NULL) {
        label_unit->setText(obj.value(QString("unit")).toString());
    }
    if (label_varName != NULL) {
        label_varName->setText(obj.value(QString("displayname")).toString());
    }

    QStandardItemModel *theModel = new QStandardItemModel();
    QList<QStandardItem *> theList;

    QJsonArray options = obj.value(QString("options")).toArray();
    QString theDefault = obj.value(QString("default")).toString();

    foreach (QJsonValue val, options)
    {
        theList.append(new QStandardItem(val.toString()));
    }
    theModel->appendRow(theList);

    qDebug() << options;
    qDebug() << theList;
    qDebug() << theModel;

    theComboBox = new QComboBox(this);
    layout->insertWidget(1, theComboBox, 4);
    theComboBox->setModel(theModel);
    theComboBox->setCurrentText(theDefault);


    this->setLayout(layout);  // do I need this one?
}

QString SCtrChoiceDataWidget::toString()
{
    QString checked = theComboBox->currentText();
    return checked;
}

