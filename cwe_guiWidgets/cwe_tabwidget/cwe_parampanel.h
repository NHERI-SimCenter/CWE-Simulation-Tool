#ifndef CWE_PARAMPANEL_H
#define CWE_PARAMPANEL_H

#include <QScrollArea>
#include <QMap>
#include <QJsonObject>
#include <SimCenter_widgets/sctrstates.h>

class SCtr_MasterDataWidget;

namespace Ui {
class CWE_ParamPanel;
}

class CWE_ParamPanel : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_ParamPanel(QWidget *parent = 0);
    ~CWE_ParamPanel();

    void setViewState(SimCenterViewState);
    SimCenterViewState getViewState();
    void setData(QJsonObject &);
    bool addVariable(QString varName, QJsonObject JSONvar, const QString &key, const QString &label, QString * setVal);

private:
    SimCenterViewState m_viewState;
    QJsonObject m_obj;
    QMap<QString, SCtr_MasterDataWidget *> *variableWidgets;
};

#endif // CWE_PARAMPANEL_H
