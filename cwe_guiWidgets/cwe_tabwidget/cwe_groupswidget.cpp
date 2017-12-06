/*
 * the CWE_StageTab represents a single tab and data field within
 * the CWE_TabWidget.
 */

#include "cwe_groupswidget.h"

#include "cwe_stagestatustab.h"
#include "SimCenter_widgets/sctrstates.h"
#include <QJsonObject>

CWE_GroupsWidget::CWE_GroupsWidget(QWidget *parent) :
    QTabWidget(parent)
{
    this->setViewState(SimCenterViewState::visible);
}

CWE_GroupsWidget::~CWE_GroupsWidget()
{

}

void CWE_GroupsWidget::setCorrespondingTab(CWE_StageStatusTab * newTab)
{
    myTab = newTab;
}

// set the group definitions as a JSon file
void CWE_GroupsWidget::setData(QJsonObject &obj)
{
    m_obj = obj;

    /*
    QJsonObject stageInfo = m_obj[name].toObject();
    QString labelText;

    labelText = stageInfo["name"].toString();
    labelText = labelText.append("\nParameters");

    // add a stage tab to ui->theTabWidget
    int idx = ui->theTabWidget->addGroupTab(name, labelText, StageState::UNRUN);
    stageTabsIndex.insert(name, idx);
    */
}

// set the view state
void CWE_GroupsWidget::setViewState(SimCenterViewState state)
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

int CWE_GroupsWidget::addGroupTab(QString key, const QString &label, StageState currentState)
{
    int index = -1;
    /*
    varTabWidgets->insert(key, new QMap<QString, QWidget *>());

    // create the tab
    CWE_StageStatusTab *newTab = new CWE_StageStatusTab(key);
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
    */

    return index;
}
