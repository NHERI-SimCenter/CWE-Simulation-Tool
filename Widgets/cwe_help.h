#ifndef CWE_HELP_H
#define CWE_HELP_H

#include <QWidget>

namespace Ui {
class CWE_help;
}

class CWE_help : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_help(QWidget *parent = 0);
    ~CWE_help();

private:
    Ui::CWE_help *ui;
};

#endif // CWE_HELP_H
