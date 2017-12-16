#ifndef SCTRBOOLDATAWIDGET_H
#define SCTRBOOLDATAWIDGET_H

#include <QWidget>
#include <SimCenter_widgets/sctrmasterdatawidget.h>

class SCtrBoolDataWidget: public SCtrMasterDataWidget
{
public:
    SCtrBoolDataWidget(QWidget *parent);
    SCtrBoolDataWidget(QJsonObject &obj, QWidget *parent);
    void setData(QJsonObject &obj);
    QString toString();
    double  toDouble();
    bool toBool();
    void setChecked();
    void setUnchecked();
    void updateValue(QString);
};

#endif // SCTRBOOLDATAWIDGET_H
