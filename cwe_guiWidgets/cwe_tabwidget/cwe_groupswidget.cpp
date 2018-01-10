/*
 * the CWE_StageTab represents a single tab and data field within
 * the CWE_TabWidget.
 */

#include "cwe_groupswidget.h"

#include "cwe_stagestatustab.h"
#include "SimCenter_widgets/sctrstates.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_parampanel.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"
#include <QJsonObject>
#include <QJsonArray>

CWE_GroupsWidget::CWE_GroupsWidget(QWidget *parent) : QTabWidget(parent)
{
    quickParameterPtr = new QMap<QString, SCtrMasterDataWidget *>();

    this->setViewState(SimCenterViewState::visible);
}

CWE_GroupsWidget::~CWE_GroupsWidget()
{

}

void CWE_GroupsWidget::setCorrespondingTab(CWE_StageStatusTab * newTab)
{
    myTab = newTab;
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

    QMapIterator<QString, SCtrMasterDataWidget *> iter(*quickParameterPtr);
    while (iter.hasNext())
    {
        iter.next();
        (iter.value())->setViewState(m_viewState);
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

void CWE_GroupsWidget::initQuickParameterPtr()
{
    quickParameterPtr->clear();

    QMap<QString, SCtrMasterDataWidget *> groupMap = this->getParameterWidgetMap();
    QMapIterator<QString, SCtrMasterDataWidget *> groupIter(groupMap);

    while (groupIter.hasNext())
    {
        groupIter.next();
        quickParameterPtr->insert(groupIter.key(), groupIter.value());
    }
}

void CWE_GroupsWidget::updateParameterValues(QMap<QString, QString> newValues)
{
    QMapIterator<QString, QString> iter(newValues);

    while (iter.hasNext())
    {
        iter.next();
        QString key = iter.key();
        if (quickParameterPtr->contains(key))
        {
            (quickParameterPtr->value(key))->updateValue(iter.value());
        }
    }
}

int CWE_GroupsWidget::collectParamData(QMap<QString, QString> &currentParameters)
{
    int count = 0;

    // collect parameter values from all SCtrMasterDataWidget objects
    QMapIterator<QString, SCtrMasterDataWidget *> iter(*quickParameterPtr);

    while (iter.hasNext())
    {
        iter.next();
        currentParameters.insert(iter.key(), (iter.value())->value());
        count++;
    }

    return count;
}

