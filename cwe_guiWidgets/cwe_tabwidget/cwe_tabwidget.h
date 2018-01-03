#ifndef CWE_TABWIDGET_H
#define CWE_TABWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>
#include <QTabWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QLayout>

#include "../cwe_defines.h"


class CWE_StageStatusTab;
class CWE_GroupsWidget;
class CWE_Parameters;
class SCtrMasterDataWidget;
class VWTinterfaceDriver;
enum class SimCenterViewState;
enum class StageState;

namespace Ui {
class CWE_TabWidget;
}

class CWE_TabWidget : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_TabWidget(QWidget *parent = 0);
    ~CWE_TabWidget();
    void setController(CWE_Parameters * newController);
    void resetView();

    void setViewState(SimCenterViewState);
    SimCenterViewState viewState();

    bool addVariable(QString varName, QJsonObject JSONvariable, const QString &key, const QString &label , QString *setVal = NULL);
    void addVarsData(QJsonObject , QJsonObject );

    void setParameterConfig(QJsonObject &obj);
    void updateParameterValues(QMap<QString, QString>);
    void initQuickParameterPtr();

    QMap<QString, QString> collectParamData();

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();

    void on_groupTabSelected(CWE_GroupsWidget *);
    void on_tabActivated(CWE_StageStatusTab *);

protected:
    void setButtonMode(uint mode);

private:
    static QString getStateText(StageState theState);
    QString getCurrentSelectedStage();

    CWE_Parameters * myController = NULL;

    Ui::CWE_TabWidget *ui;

    SimCenterViewState m_viewState;

    QMap<QString, CWE_StageStatusTab *> *stageTabList;
    QMap<QString, SCtrMasterDataWidget *> *quickParameterPtr;
};

#endif // CWE_TABWIDGET_H
