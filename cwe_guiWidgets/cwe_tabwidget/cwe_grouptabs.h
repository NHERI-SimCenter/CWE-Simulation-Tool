#ifndef CWE_TAB_H
#define CWE_TAB_H

#include <QJsonObject>
#include <QTabWidget>
#include <SimCenter_widgets/sctrstates.h>
#include <cwe_guiWidgets/cwe_withstatusbutton.h>

class CWE_GroupTabs
{
public:
    CWE_GroupTabs();
    ~CWE_GroupTabs();
    void setData(QJsonObject &obj);         // set the group definitions as a JSon file
    QJsonObject getData(QJsonObject &obj);  // set the group definitions as a JSon file
    void setViewState(SimCenterViewState);  // set the view state
    SimCenterViewState viewState();         // return current view state

protected:
    CWE_GroupTabs *theGroupTabs();  // returns pointer to group tab widget

private:
    QTabWidget *m_tabWidget = NULL;
    CWE_WithStatusButton *m_statusButton = NULL;

    SimCenterViewState m_viewState;
    QJsonObject m_obj;
};

#endif // CWE_TAB_H
