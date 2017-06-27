#ifndef BASEANALYSISTYPE_H
#define BASEANALYSISTYPE_H

#include <QObject>
#include <QWidget>
#include <QJsonObject>

typedef struct {
    QString label;  // display label
    QString sValue; // alphanumeric value
    float   fValue; // float value
    int     iValue; // integer value
    QString type;   // property type
} PARAMETER_VALUES;

class BaseAnalysisType
{
public:
    BaseAnalysisType();
    ~BaseAnalysisType();
    QVector<PARAMETER_VALUES *> * ParameterList();
    bool setParemeterList(QVector<PARAMETER_VALUES *>);

signals:

private slots:

private:
    QVector<PARAMETER_VALUES *> * params;
    PARAMETER_VALUES * newparameter;
    QJsonObject      * JSONparameters;

};

#endif // BASEANALYSISTYPE_H
