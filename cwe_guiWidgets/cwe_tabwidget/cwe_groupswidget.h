#ifndef CWE_GROUPSWIDGET_H
#define CWE_GROUPSWIDGET_H

#include <QJsonObject>
#include <QTabWidget>

class CWE_StageStatusTab;
class CWE_ParamTab;
enum class SimCenterViewState;
enum class StageState;

class CWE_GroupsWidget : public QTabWidget
{
public:
    CWE_GroupsWidget(QWidget *parent = NULL);
    ~CWE_GroupsWidget();
    void setCorrespondingTab(CWE_StageStatusTab * newTab);

    void setData(QJsonObject &obj);         // set the group definitions as a JSon file
    void setViewState(SimCenterViewState);  // set the view state
    int  addGroupTab(QString key, const QString &label, StageState currentState);

protected:
    CWE_ParamTab *getGroupTab();  // returns pointer to group tab widget

private:
    SimCenterViewState m_viewState;
    QJsonObject m_obj;

    CWE_StageStatusTab * myTab;
};

#endif // CWE_GROUPSWIDGET_H
