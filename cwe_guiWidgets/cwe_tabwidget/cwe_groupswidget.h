#ifndef CWE_GROUPSWIDGET_H
#define CWE_GROUPSWIDGET_H

#include <QJsonObject>;
class QTabWidget;
class CWE_StageStatusTab;
class CWE_ParamTab;
enum class SimCenterViewState;
enum class StageState;

class CWE_GroupsWidget : QTabWidget
{
public:
    CWE_GroupsWidget(QWidget *parent = NULL);
    ~CWE_GroupsWidget();
    void setData(QJsonObject &obj);         // set the group definitions as a JSon file
    QJsonObject getData();                  // set the group definitions as a JSon file
    void setViewState(SimCenterViewState);  // set the view state
    SimCenterViewState viewState();         // return current view state
    int  addGroupTab(QString key, const QString &label, StageState currentState);

protected:
    CWE_ParamTab *getGroupTab();  // returns pointer to group tab widget

private:
    QTabWidget *m_tabWidget;
    CWE_StageStatusTab *m_statusButton;

    SimCenterViewState m_viewState;
    QJsonObject m_obj;
};

#endif // CWE_GROUPSWIDGET_H
