/*
 * the CWE_ParamPanel is a scoll area that holds the individual
 * variable input/display objects (SCtr_MasterDataWidget and its derivates)
 */

#include "cwe_parampanel.h"
#include "cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>

#include "SimCenter_widgets/sctrstddatawidget.h"
#include "SimCenter_widgets/sctrbooldatawidget.h"
#include "SimCenter_widgets/sctrfiledatawidget.h"
#include "SimCenter_widgets/sctrchoicedatawidget.h"

#include <QDebug>


CWE_ParamPanel::CWE_ParamPanel(QWidget *parent) :
    QFrame(parent)
{
    variableWidgets = new QMap<QString, SCtrMasterDataWidget *>();
    this->setViewState(SimCenterViewState::visible);
}

CWE_ParamPanel::~CWE_ParamPanel()
{
    if (variableWidgets != NULL) delete variableWidgets;
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

SCtrMasterDataWidget * CWE_ParamPanel::addVariable(QString varName, QJsonObject &theVariable)
{
    SCtrMasterDataWidget *theVar = NULL;

    QLayout *layout = this->layout();

    QString type = theVariable.value("type").toString();

    if (type.toLower() == "std") {
        theVar = new SCtrStdDataWidget(theVariable, this);
        layout->addWidget(theVar);
    }
    else if (type.toLower() == "choose") {
        theVar = new SCtrChoiceDataWidget(theVariable, this);
        layout->addWidget(theVar);
    }
    else if (type.toLower() == "bool") {
        theVar = new SCtrBoolDataWidget(theVariable, this);
        layout->addWidget(theVar);
    }
    else if (type.toLower() == "file") {
        theVar = new SCtrFileDataWidget(theVariable, this);
        layout->addWidget(theVar);
    }
    else {
        /* add an error message */
        QMessageBox *msg = new QMessageBox(QMessageBox::Information,
                                      QString("Warning"),
                                      QString("Variable %1 of unknown type %2.\nVariable ignored.").arg(varName).arg(type));
        msg->exec();
        delete msg;
    }

    variableWidgets->insert(varName, theVar);

    return theVar;

}

void CWE_ParamPanel::addParameterConfig(QJsonArray &groupVars, QJsonObject &allVars)
{
    QVBoxLayout *layout = (QVBoxLayout *)this->layout();
    if (layout != NULL) { delete layout; }
    layout = new QVBoxLayout();
    this->setLayout(layout);

    foreach (QJsonValue var, groupVars)
    {
        QString varName = var.toString();
        QJsonObject theVariable = allVars.value(varName).toObject();
        SCtrMasterDataWidget *varWidget = this->addVariable(varName, theVariable);
    }

    layout->addStretch(1);
}


QMap<QString, SCtrMasterDataWidget *> CWE_ParamPanel::getParameterWidgetMap()
{
    QMap<QString, SCtrMasterDataWidget *> panelMap;

    QMapIterator<QString, SCtrMasterDataWidget *> variablesIter(*variableWidgets);

    while (variablesIter.hasNext())
    {
        variablesIter.next();
        panelMap.insert(variablesIter.key(), variablesIter.value());
    }

    return panelMap;
}
