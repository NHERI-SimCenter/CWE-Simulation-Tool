#include "sctrvalidators.h"

#include <QValidator>


SCtrNoValidator::SCtrNoValidator(QObject *parent):
    QValidator(parent)
{

}

QValidator::State SCtrNoValidator::validate(QString &, int &) const
{
    return QValidator::Acceptable;
}

SCtrBoolValidator::SCtrBoolValidator(QObject *parent):
    QValidator(parent)
{

}

QValidator::State SCtrBoolValidator::validate(QString &input, int &) const
{
    if (input.toLower() == "f")     return QValidator::Acceptable;
    if (input.toLower() == "fa")    return QValidator::Acceptable;
    if (input.toLower() == "fal")   return QValidator::Acceptable;
    if (input.toLower() == "fals")  return QValidator::Acceptable;
    if (input.toLower() == "false") return QValidator::Acceptable;
    if (input.toLower() == "t")     return QValidator::Acceptable;
    if (input.toLower() == "tr")    return QValidator::Acceptable;
    if (input.toLower() == "tru")   return QValidator::Acceptable;
    if (input.toLower() == "true")  return QValidator::Acceptable;

    return QValidator::Invalid;
}
