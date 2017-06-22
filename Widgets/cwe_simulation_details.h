#ifndef CWE_SIMULATION_DETAILS_H
#define CWE_SIMULATION_DETAILS_H

#include <QWidget>

namespace Ui {
class CWE_simulation_details;
}

class CWE_simulation_details : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_simulation_details(QWidget *parent = 0);
    ~CWE_simulation_details();

private:
    Ui::CWE_simulation_details *ui;
};

#endif // CWE_SIMULATION_DETAILS_H
