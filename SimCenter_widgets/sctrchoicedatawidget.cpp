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

    foreach (QString key, options.keys())
    {
        QList<QStandardItem *> newRow;
        // newRow.clear();
        newRow.append(new QStandardItem(key));
        newRow.append(new QStandardItem(options.value(key).toString()));

        theModel->appendRow(newRow);
    }

    theComboBox = new QComboBox(this);
    layout->insertWidget(1, theComboBox, 4);
    theComboBox->setModel(theModel);
    theComboBox->setModelColumn(1);
    theComboBox->setCurrentText(options.value(theDefault).toString());

    this->setLayout(layout);
}

QString SCtrChoiceDataWidget::toString()
{
    /* *** update the value ***
     *
     * the model contains two columns:
     *   col 0: the variable name
     *   col 1: the human readable variable description
     *
     * theComboBox displays col 1
     * the driver uses the associated value from col 0
     * thus, the return value is the associate QString from col 0
     */

    QStandardItemModel *model = (QStandardItemModel *)theComboBox->model();
    QStandardItem *item = model->item(theComboBox->currentIndex(), 0);

    return item->text();
}

void SCtrChoiceDataWidget::updateValue(QString s)
{
    /* *** update the value ***
     *
     * the model contains two columns:
     *   col 0: the variable name
     *   col 1: the human readable variable description
     *
     * theComboBox displays col 1
     * the driver uses the associated value from col 0
     */


    /* check if new information is an appropriate type */
    QStandardItemModel *theModel = (QStandardItemModel *)theComboBox->model();
    QList<QStandardItem *> itemList = theModel->findItems(s, Qt::MatchExactly, 0);

    if (itemList.isEmpty())
    {
        /* add an error message */
        QString name = m_obj["displayname"].toString();
        cwe_globals::displayPopup(QString("Variable %1 of unknown selection option \'%2\'.\nVariable ignored.").arg(name).arg(s), "Warning");
        return;
    }

    /* the following loop should never have more than one item in the itemList ...  */
    foreach (QStandardItem *item, itemList)
    {
        QModelIndex idx = theModel->indexFromItem(item);
        theComboBox->setCurrentIndex(idx.row());
    }

}

