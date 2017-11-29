/*
 * the CWE_ParamPanel is a scoll area that holds the individual
 * variable input/display objects (SCtr_MasterDataWidget and its derivates)
 */

#include "cwe_parampanel.h"
#include "ui_cwe_parampanel.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

CWE_ParamPanel::CWE_ParamPanel(QWidget *parent) :
    QScrollArea(parent),
    ui(new Ui::CWE_ParamPanel)
{
    ui->setupUi(this);
    variableWidgets = new QMap<QString, SCtr_MasterDataWidget *>();
    this->setViewState(SimCenterViewState::visible);
}

CWE_ParamPanel::~CWE_ParamPanel()
{
    delete ui;

    if (variableWidgets != NULL) delete variableWidgets;
}

QWidget * CWE_ParamPanel::getParameterSpace()
{
    return ui->parameterSpace;
}

void CWE_ParamPanel::setData(QJsonObject &obj)
{
    m_obj = obj;
}

void CWE_ParamPanel::setViewState(SimCenterViewState state)
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

SimCenterViewState CWE_ParamPanel::getViewState()
{
    return m_viewState;
}

bool CWE_ParamPanel::addVariable(QString varName, QJsonObject JSONvar, const QString &key, const QString &label, QString * setVal)
{
    QString type = JSONvar["type"].toString();
    if (type == "") {
        return false;
    }
    else
    {
        //QWidget *parent = varTabWidgets->value(key)->value(label);
        QWidget *parent = this;
        if (parent != NULL)
        {
            /* temporary disabled */
            //this->addType(varName, type, JSONvar, parent, setVal);
            return true;
        }
        else { return false; }
    }
}
