#ifndef CWE_PARAMPANEL_H
#define CWE_PARAMPANEL_H

#include <QScrollArea>
#include <QMap>
#include <QJsonObject>
#include <QLayout>
#include <QMessageBox>
#include <QDebug>
#include <QJsonArray>

class SCtrMasterDataWidget;
enum class SimCenterViewState;

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
    SCtrMasterDataWidget * addVariable(QString varName, QJsonObject &theVariable);
    void addParameterConfig(QJsonArray &groupVars, QJsonObject &allVars);
    QMap<QString, SCtrMasterDataWidget *> getParameterWidgetMap();

private:
    SimCenterViewState m_viewState;
    QJsonObject m_obj;
    QMap<QString, SCtrMasterDataWidget *> *variableWidgets;
};

#endif // CWE_PARAMPANEL_H
