/*
 * the CWE_StageTab represents a single tab and data field within
 * the CWE_TabWidget.
 */

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
QJsonObject CWE_StageTab::getData(QJsonObject &obj)
{

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



int CWE_StageTab::addGroupTab(QString key, const QString &label, StageState currentState)
{
    varTabWidgets->insert(key, new QMap<QString, QWidget *>());

    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton(key);
    newTab->setText(label);

    newTab->setStatus(getStateText(currentState));
    int index = ui->verticalTabLayout->count()-1;
    newTab->setIndex(index);
    ui->verticalTabLayout->insertWidget(index, newTab);

    groupWidget->insert(key, newTab);

    connect(newTab,SIGNAL(btn_pressed(int,QString)),this,SLOT(on_groupTabSelected(int, QString)));
    //connect(newTab,SIGNAL(btn_released(int)),this,SLOT(on_groupTabSelected(int)));

    // create the widget to hold the parameter input
    QTabWidget *pWidget = new QTabWidget();
    ui->stackedWidget->insertWidget(index, pWidget);

    groupTabList->insert(key, pWidget);

    return index;
}
