#include "sctrvalidators.h"

#include <QValidator>


SCtrNoValidator::SCtrNoValidator(QObject *parent):
    QValidator(parent)
{

};

SCtrBoolValidator::SCtrBoolValidator(QObject *parent):
    QValidator(parent)
{

}

QValidator::State SCtrBoolValidator::validate(QString &input, int &pos) const
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
};
