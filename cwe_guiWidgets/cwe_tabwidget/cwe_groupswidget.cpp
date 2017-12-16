/*
 * the CWE_StageTab represents a single tab and data field within
 * the CWE_TabWidget.
 */

#include "cwe_groupswidget.h"

#include "cwe_stagestatustab.h"
#include "SimCenter_widgets/sctrstates.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_parampanel.h"
#include <QJsonObject>
#include <QJsonArray>

CWE_GroupsWidget::CWE_GroupsWidget(QWidget *parent) : QTabWidget(parent)
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
    */

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

    QObject::connect(newTab,SIGNAL(btn_pressed(CWE_GroupsWidget *,QString)),this,SLOT(on_groupTabSelected(CWE_GroupsWidget *, QString)));
    //QObject::connect(newTab,SIGNAL(btn_released(CWE_GroupsWidget *)),this,SLOT(on_groupTabSelected(CWE_GroupsWidget *)));

    // create the widget to hold the parameter input
    QTabWidget *pWidget = new QTabWidget();
    ui->stackedWidget->insertWidget(index, pWidget);

    groupTabList->insert(key, pWidget);
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

void CWE_GroupsWidget::setParameterConfig(QString key, QJsonObject &obj)
{
    /* find all groups and create a tab per group */
    QJsonArray groups = obj.value(QString("stages")).toObject().value(key).toObject().value(QString("groups")).toArray();

    foreach (QJsonValue group, groups)
    {
        QString groupName = group.toString();
        QScrollArea *scrollArea = new QScrollArea(this);
        CWE_ParamPanel *panel = new CWE_ParamPanel(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(panel);

        this->addTab(scrollArea, groupName);

        /* now add the parameter tabs */
        QJsonArray groupVars = obj.value(QString("varGroups")).toObject().value(groupName).toArray();
        QJsonObject allVars  = obj.value(QString("vars")).toObject();
        panel->addParameterConfig(groupVars, allVars);
    }
}

void CWE_GroupsWidget::linkWidget(CWE_StageStatusTab *tab)
{
    myTab = tab;
}


QMap<QString, SCtrMasterDataWidget *> CWE_GroupsWidget::getParameterWidgetMap()
{
    QMap<QString, SCtrMasterDataWidget *> groupMap;

    for (int i=0; i<this->count(); i++)
    {
        CWE_ParamPanel * panel = (CWE_ParamPanel *)((QScrollArea *)(this->widget(i)))->widget();
        QMap<QString, SCtrMasterDataWidget *> panelParams = panel->getParameterWidgetMap();
        QMapIterator<QString, SCtrMasterDataWidget *> panelParamIter(panelParams);
        while (panelParamIter.hasNext())
        {
            panelParamIter.next();
            groupMap.insert(panelParamIter.key(), panelParamIter.value());
        }
    }

    return groupMap;
}
