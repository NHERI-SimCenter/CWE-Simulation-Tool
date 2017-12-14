#ifndef SCTRFILEDATAWIDGET_H
#define SCTRFILEDATAWIDGET_H

#include <QWidget>
#include <SimCenter_widgets/sctrmasterdatawidget.h>

class SCtrFileDataWidget: public SCtrMasterDataWidget
{
public:
    SCtrFileDataWidget(QWidget *parent);
    SCtrFileDataWidget(QJsonObject &obj, QWidget *parent);
    void setData(QJsonObject &obj);
    QString toString();
    double  toDouble();
    bool toBool();
};

#endif // SCTRFILEDATAWIDGET_H
