#ifndef SCTRCHOICEDATAWIDGET_H
#define SCTRCHOICEDATAWIDGET_H

#include <QWidget>
#include <SimCenter_widgets/sctrmasterdatawidget.h>

class SCtrChoiceDataWidget: public SCtrMasterDataWidget
{
public:
    SCtrChoiceDataWidget(QWidget *parent);
    SCtrChoiceDataWidget(QJsonObject &obj, QWidget *parent);
    void setData(QJsonObject &obj);
    QString toString();
    void updateValue(QString);
};

#endif // SCTRCHOICEDATAWIDGET_H
