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
    JSONparameters->insert(QString('analysisType'), QJsonValue('Channel flow'));

    /* set parameter list for this analysis type */
    // analysis type

    JSONparameters->insert(QString('parameters'), JSONparameterList);
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

}
