#include "cwe_parametertab.h"
#include "ui_cwe_parametertab.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_stagetab.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

CWE_ParameterTab::CWE_ParameterTab(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::CWE_ParameterTab)
{
    ui->setupUi(this);
    variableWidgets = new QMap<QString, SCtr_MasterDataWidget *>();
    this->setViewState(SimCenterViewState::visible);
}

CWE_ParameterTab::~CWE_ParameterTab()
{
    delete ui;
    if (variableWidgets != NULL) delete variableWidgets;
}

QWidget * CWE_ParameterTab::getParameterSpace()
{
    return ui->parameterSpace;
}

void CWE_ParameterTab::setData(QJsonObject &obj)
{
    m_obj = obj;
}

void CWE_ParameterTab::setViewState(SimCenterViewState state)
{
    switch (state)
    {
    case SimCenterViewState::editable:
        m_viewState = SimCenterViewState::editable;
        break;
    case SimCenterViewState::hidden:
        m_viewState = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::visible:
    default:
        m_viewState = SimCenterViewState::visible;
    }
}

SimCenterViewState CWE_ParameterTab::getViewState()
{
    return m_viewState;
}
