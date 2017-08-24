#ifndef CWE_PARAMETERTAB_H
#define CWE_PARAMETERTAB_H

#include <QWidget>

namespace Ui {
class CWE_ParameterTab;
}

class CWE_ParameterTab : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_ParameterTab(QWidget *parent = 0);
    ~CWE_ParameterTab();

private:
    Ui::CWE_ParameterTab *ui;
};

#endif // CWE_PARAMETERTAB_H
