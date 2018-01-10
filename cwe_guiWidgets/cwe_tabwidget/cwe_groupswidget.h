#ifndef CWE_GROUPSWIDGET_H
#define CWE_GROUPSWIDGET_H

#include <QJsonObject>
#include <QTabWidget>

class CWE_StageStatusTab;
class CWE_ParamTab;
class SCtrMasterDataWidget;
enum class SimCenterViewState;
enum class StageState;

class CWE_GroupsWidget : public QTabWidget
{
public:
    CWE_GroupsWidget(QWidget *parent = NULL);
    ~CWE_GroupsWidget();
    void setCorrespondingTab(CWE_StageStatusTab * newTab);

    void setViewState(SimCenterViewState);  // set the view state
    void addVSpacer(const QString &key, const QString &label);
    void addVarsToTab(QString key, const QString &label, QJsonArray *, QJsonObject *, QMap<QString,QString> * );

    void setParameterConfig(QString key, QJsonObject &obj);
    void linkWidget(CWE_StageStatusTab *tab);
    QMap<QString, SCtrMasterDataWidget *> getParameterWidgetMap();

    void initQuickParameterPtr();
    void updateParameterValues(QMap<QString, QString> );
    int collectParamData(QMap<QString, QString> &);


protected:
    CWE_ParamTab *getGroupTab();  // returns pointer to group tab widget

private:
    SimCenterViewState m_viewState;
    QJsonObject m_obj;

    CWE_StageStatusTab * myTab;

    QMap<QString, SCtrMasterDataWidget *> *quickParameterPtr;
};

#endif // CWE_GROUPSWIDGET_H
