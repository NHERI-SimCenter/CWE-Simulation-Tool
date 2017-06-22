#ifndef CWE_LANDING_H
#define CWE_LANDING_H

#include <QWidget>

namespace Ui {
class CWE_landing;
}

class CWE_landing : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_landing(QWidget *parent = 0);
    ~CWE_landing();

private:
    Ui::CWE_landing *ui;
};

#endif // CWE_LANDING_H
