#include "cwe_stagetab.h"

#include "cwe_withstatusbutton.h"
#include "SimCenter_widgets/sctrstates.h"
#include <QJsonObject>

CWE_StageTab::CWE_StageTab()
{

    m_tabWidget = NULL;
    m_statusButton = NULL;
    this->setViewState(SimCenterViewState::visible);
}

CWE_StageTab::~CWE_StageTab()
{
    if (m_tabWidget != NULL) {
        delete m_tabWidget;
        m_tabWidget = NULL;
    }
    if (m_statusButton != NULL) {
        delete m_statusButton;
        m_statusButton = NULL;
    }
}

// set the group definitions as a JSon file
void CWE_StageTab::setData(QJsonObject &obj)
{
    m_obj = obj;
}

// set the group definitions as a JSon file
QJsonObject CWE_StageTab::getData()
{
    return QJsonObject(m_obj);
}

// set the view state
void CWE_StageTab::setViewState(SimCenterViewState state)
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
SimCenterViewState CWE_StageTab::viewState()
{
    return m_viewState;
}
