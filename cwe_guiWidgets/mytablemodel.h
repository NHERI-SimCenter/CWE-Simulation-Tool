#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

class QStringList;
#include <QAbstractTableModel>

class MyTableModel : public QAbstractTableModel
{
public:
  MyTableModel(QWidget *parent);

  int rowCount(const QModelIndex& parent) const;
  int columnCount(const QModelIndex& parent) const;
  QVariant data(const QModelIndex& index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  void addStringList(QStringList& list);
  void setHeaders(QStringList &list) {ColumnLabels=list.toVector(); numColumns=ColumnLabels.size();}

protected:
  QVector<QVector<QString>> Rows;
  int numColumns;
  int numRows;
  QVector<QString> ColumnLabels;
};

#endif // MYTABLEMODEL_H
