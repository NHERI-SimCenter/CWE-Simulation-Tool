#ifndef SCTRSTDDATAWIDGET_H
#define SCTRSTDDATAWIDGET_H

#include <QWidget>
#include <SimCenter_widgets/sctrmasterdatawidget.h>

class SCtrStdDataWidget: public SCtrMasterDataWidget
{
public:
    SCtrStdDataWidget(QWidget *parent);
    void setData(QJsonObject &obj);
};

#endif // SCTRSTDDATAWIDGET_H
