#ifndef SCTRVALIDATORS_H
#define SCTRVALIDATORS_H

#include <QValidator>

class SCtrNoValidator: public QValidator
{
public:
    SCtrNoValidator(QObject *parent = 0);
    // ~SCtrNoValidator();
    void fixup(QString &str) const {};
    QValidator::State validate(QString &input, int &pos) const { return QValidator::Acceptable; };
};


class SCtrBoolValidator: public QValidator
{
public:
    SCtrBoolValidator(QObject *parent = 0);
    // ~SCtrBoolValidator();
    void fixup(QString &str) const {};
    QValidator::State validate(QString &input, int &pos) const;

protected:

private:

};

#endif // SCTRVALIDATORS_H
