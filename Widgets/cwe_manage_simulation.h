#ifndef CWE_MANAGE_SIMULATION_H
#define CWE_MANAGE_SIMULATION_H

#include <QWidget>

namespace Ui {
class CWE_manage_simulation;
}

class CWE_manage_simulation : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_manage_simulation(QWidget *parent = 0);
    ~CWE_manage_simulation();

private:
    Ui::CWE_manage_simulation *ui;
};

#endif // CWE_MANAGE_SIMULATION_H
