#include "cwe_grouptabs.h"

CWE_GroupTabs::CWE_GroupTabs()
{
    this->setViewState(SimCenterViewState::visible);
}

CWE_GroupTabs::~CWE_GroupTabs()
{
    if (m_tabWidget != NULL) delete m_tabWidget;
    if (m_statusButton != NULL) delete m_statusButton;
}

// set the group definitions as a JSon file
void CWE_GroupTabs::setData(QJsonObject &obj)
{
    m_obj = obj;
}

// set the group definitions as a JSon file
QJsonObject CWE_GroupTabs::getData(QJsonObject &obj)
{

}

// set the view state
void CWE_GroupTabs::setViewState(SimCenterViewState state)
{
    switch (state) {
    case SimCenterViewState::hidden:
        m_viewState = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::editable:
        m_viewState = SimCenterViewState::editable;
        break;
    case SimCenterViewState::visible:
    default:
        m_viewState = SimCenterViewState::visible;
        break;
    }
}

// return current view state
SimCenterViewState CWE_GroupTabs::viewState()
{
    return m_viewState;
}
