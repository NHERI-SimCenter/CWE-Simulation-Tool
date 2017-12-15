#ifndef SCTRSTDDATAWIDGET_H
#define SCTRSTDDATAWIDGET_H

#include <QWidget>
#include <SimCenter_widgets/sctrmasterdatawidget.h>

class SCtrStdDataWidget: public SCtrMasterDataWidget
{
public:
    SCtrStdDataWidget(QWidget *parent);
    SCtrStdDataWidget(QJsonObject &obj, QWidget *parent);
    void setData(QJsonObject &obj);
    QString toString();
    double  toDouble();
    QString value();
};

#endif // SCTRSTDDATAWIDGET_H
