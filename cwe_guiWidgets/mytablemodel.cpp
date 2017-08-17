#include "mytablemodel.h"

#include <iostream>
#include <sstream>
#include <QTime>


MyTableModel::MyTableModel(QWidget *parent) : QAbstractTableModel()
{
  QVector<QString> row0;
  row0.append("some URL");
  row0.append("none");
  row0.append(QTime::currentTime().toString());
  row0.append("007");
  row0.append("SimCenter super app");

  Rows.append(row0);
}

int MyTableModel::rowCount(const QModelIndex& parent) const
{
  return Rows.size();
}

int MyTableModel::columnCount(const QModelIndex& parent) const
{
  return Rows[0].size();
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
      switch (section) {
      case 0:  return QString("name given"); break;
      case 1:  return QString("state"); break;
      case 2:  return QString("time created"); break;
      case 3:  return QString("ID"); break;
      case 4:  return QString("application"); break;
      default: return QString("unknown");
      }

      ss << "H_" << section;
      return QString(ss.str().c_str());
      }
    else if(orientation == Qt::Vertical)
      {
      ss << section;
      return QString(ss.str().c_str());
      }

    }

  return QVariant::Invalid;
}

void MyTableModel::addStringList(QStringList &list)
{

}
