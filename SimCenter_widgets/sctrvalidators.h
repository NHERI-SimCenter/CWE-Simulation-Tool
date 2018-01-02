#ifndef SCTRVALIDATORS_H
#define SCTRVALIDATORS_H

#include <QValidator>

class SCtrNoValidator: public QValidator
{
public:
    SCtrNoValidator(QObject *parent = 0);
    // ~SCtrNoValidator();
    QValidator::State validate(QString &input, int &pos) const;
};


class SCtrBoolValidator: public QValidator
{
public:
    SCtrBoolValidator(QObject *parent = 0);
    // ~SCtrBoolValidator();
    QValidator::State validate(QString &input, int &pos) const;

protected:

private:

};

#endif // SCTRVALIDATORS_H
