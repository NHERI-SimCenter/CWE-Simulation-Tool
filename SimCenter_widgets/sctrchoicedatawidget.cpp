#include "sctrchoicedatawidget.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QStandardItemModel>

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
    m_obj = obj;

    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    if (label_unit != NULL) {
        label_unit->setText(obj.value(QString("unit")).toString());
    }
    if (label_varName != NULL) {
        label_varName->setText(obj.value(QString("displayname")).toString());
    }

    QStandardItemModel *theModel = new QStandardItemModel(this);

    QJsonObject options = obj.value(QString("options")).toObject();
    QString theDefault = obj.value(QString("default")).toString();

    //TODO: This is wrong, looks at human name and not internal name
    foreach (QJsonValue val, options)
    {
        theModel->appendRow(new QStandardItem(val.toString()));
    }

    theComboBox = new QComboBox(this);
    layout->insertWidget(1, theComboBox, 4);
    theComboBox->setModel(theModel);
    theComboBox->setCurrentText(theDefault);

    this->setLayout(layout);  // do I need this one?
}

QString SCtrChoiceDataWidget::toString()
{
    QString selection = theComboBox->currentText();
    return selection;
}

void SCtrChoiceDataWidget::updateValue(QString s)
{
    /* check if new information is an appropriate type */
    QStandardItemModel *theModel = (QStandardItemModel *)theComboBox->model();
    QList<QStandardItem *> itemList = theModel->findItems(s);
    if (itemList.isEmpty())
    {
        /* add an error message */
        QString name = m_obj["displayname"].toString();
        cwe_globals::displayPopup(QString("Variable %1 of unknown selection option \'%2\'.\nVariable ignored.").arg(name).arg(s), "Warning");
        return;
    }

    /* update the value */
    theComboBox->setCurrentText(s);
}

