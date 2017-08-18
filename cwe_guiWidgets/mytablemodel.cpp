#include "mytablemodel.h"

#include <iostream>
#include <sstream>
#include <QTime>


MyTableModel::MyTableModel(QWidget *parent) : QAbstractTableModel()
{
    this->insertColumn(0);
    numColumns = 1;
    numRows    = 0;
}

int MyTableModel::rowCount(const QModelIndex& parent) const
{
    return Rows.size();
}

int MyTableModel::columnCount(const QModelIndex& parent) const
{
    return numColumns;
}

QVariant MyTableModel::data(const QModelIndex& index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        return Rows[index.row()][index.column()];
    }
    return QVariant::Invalid;
}

QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if(role == Qt::DisplayRole)
    {
        std::stringstream ss;
        if(orientation == Qt::Horizontal)
        {
            if (section < ColumnLabels.size()) {
                return ColumnLabels[section];

            }
            else {
                return QString("use MyTableModel::setHeaders(QStringList)");
            }
        }
        else if(orientation == Qt::Vertical)
        {
            ss << section;
            return QString(ss.str().c_str());
            //return QString("");
        }

    }

    return QVariant::Invalid;
}

void MyTableModel::addStringList(QStringList &list)
{
    Rows.append(list.toVector());
    numRows++;
}


