/*
 * the CWE_StageTab represents a single tab and data field within
 * the CWE_TabWidget.
 */

#include "cwe_groupswidget.h"

#include "cwe_stagestatustab.h"
#include "SimCenter_widgets/sctrstates.h"
#include <QJsonObject>

CWE_StageTab::CWE_StageTab(QWidget *parent) :
    QTabWidget(parent)
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

    QJsonObject stageInfo = m_obj[name].toObject();
    QString labelText;

    labelText = stageInfo["name"].toString();
    labelText = labelText.append("\nParameters");

    // add a stage tab to ui->theTabWidget
    int idx = ui->theTabWidget->addGroupTab(name, labelText, StageState::UNRUN);
    stageTabsIndex.insert(name, idx);
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



int CWE_StageTab::addGroupTab(QString key, const QString &label, StageState currentState)
{
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

    return index;
}

int CWE_StageTab::addVarTab(QString key, const QString &label)
{
    // create the widget to hold the parameter input

    CWE_ParameterTab *itm = new CWE_ParameterTab(this);
    itm->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    itm->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    itm->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);

    QGridLayout *lyt = new QGridLayout();
    itm->setLayout(lyt);

    //varTabWidgets->value(key)->insert(label, itm);

    QTabWidget * qf = groupTabList->value(key);
    int index = qf->addTab(itm, label);

    return index;
}
