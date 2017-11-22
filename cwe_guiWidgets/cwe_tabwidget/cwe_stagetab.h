#ifndef CWE_STAGETAB_H
#define CWE_STAGETAB_H

#include <QJsonObject>;
class QTabWidget;
enum class SimCenterViewState;
enum class StageState;

class CWE_WithStatusButton;

class CWE_StageTab
{
public:
    CWE_StageTab();
    ~CWE_StageTab();
    void setData(QJsonObject &obj);         // set the group definitions as a JSon file
    QJsonObject getData();                  // set the group definitions as a JSon file
    void setViewState(SimCenterViewState);  // set the view state
    SimCenterViewState viewState();         // return current view state
    int  addGroupTab(QString key, const QString &label, StageState currentState);

protected:
    CWE_StageTab *getGroupTab();  // returns pointer to group tab widget

private:
    QTabWidget *m_tabWidget;
    CWE_WithStatusButton *m_statusButton;

    SimCenterViewState m_viewState;
    QJsonObject m_obj;
};

#endif // CWE_STAGETAB_H
