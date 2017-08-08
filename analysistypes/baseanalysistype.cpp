/*********************************************************************************
**
** Copyright (c) 2017 The Regents of the University of California
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

#include "baseanalysistype.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

BaseAnalysisType::BaseAnalysisType()
{
    params = new QVector<PARAMETER_VALUES *>;

    QJsonArray JSONparameterList;
    QJsonObject item;

    JSONparameters = new QJsonObject;
    JSONparameters->insert(QString("analysisType"), QJsonValue("Channel flow"));

    /* set parameter list for this analysis type */
    // analysis type

    JSONparameters->insert(QString("parameters"), JSONparameterList);
}

BaseAnalysisType::~BaseAnalysisType()
{
    if (params) delete params;
}

QVector<PARAMETER_VALUES *> * BaseAnalysisType::ParameterList()
{
    // 1st parameter
    newparameter = new PARAMETER_VALUES;
    newparameter->label = "type";
    newparameter->type  = "string";  // "int"||"float"||"string"
    newparameter->fValue = 0.0; // ignored
    newparameter->iValue = 0;   // ignored
    newparameter->sValue = "Channel flow";
    params->append(newparameter);

    // 2nd parameter
    newparameter = new PARAMETER_VALUES;
    newparameter->label = "a";
    newparameter->type  = "";  // "int"||"float"||"string"
    newparameter->fValue = 1.0;
    newparameter->iValue = 0;   // ignored
    newparameter->sValue = "";  // ignored
    params->append(newparameter);

    // 3rd ...

    return params;
}

bool BaseAnalysisType::setParemeterList(QVector<PARAMETER_VALUES *>)
{
    //TODO: Implement
    return false;
}
