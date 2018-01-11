/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:

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

void CWE_ParamPanel::addVariable(QString varName, QJsonObject &theVariable)
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
        cwe_globals::displayPopup(QString("Variable %1 of unknown type %2.\nVariable ignored.").arg(varName).arg(type), "Warning");
        return;
    }

    variableWidgets->insert(varName, theVar);
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
        this->addVariable(varName, theVariable);
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
