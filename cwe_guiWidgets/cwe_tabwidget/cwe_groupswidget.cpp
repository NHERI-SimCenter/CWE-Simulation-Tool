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

int CWE_GroupsWidget::addVarTab(QString key, const QString &label)
{
    // create the widget to hold the parameter input

    /*
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
    */
    return -1;
}

void CWE_GroupsWidget::addVSpacer(const QString &key, const QString &label)
{
    /*
    QWidget *parent = varTabWidgets->value(key)->value(label);
    if (parent != NULL)
    {
        QGridLayout *layout = (QGridLayout*)(parent->layout());
        layout->addItem(new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount(), 2);
    }
    */
}

void CWE_GroupsWidget::addVarsToTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString,QString> * setVars)
{
    //QTabWidget *groupTab = groupTabList->value(key);
    //QWidget    *varTab   = varTabWidgets->value(key)->value(label);

    /*
    foreach (const QJsonValue &item, *varList)
    {
        QString varKey = item.toString();
        QJsonObject variableObject = (*varsInfo)[varKey].toObject();
        QString setVal;

        if (setVars->contains(varKey))
        {
            setVal = setVars->value(varKey);
            this->addVariable(varKey, variableObject, key, label, &setVal);
        }
        else
        {
            this->addVariable(varKey, variableObject, key, label, NULL);
        }
    }
    this->addVSpacer(key, label);
    */
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
