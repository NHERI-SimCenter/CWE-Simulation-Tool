#ifndef CWE_CREATE_SIMULATION_H
#define CWE_CREATE_SIMULATION_H

#include <QWidget>

namespace Ui {
class CWE_create_simulation;
}

class CWE_create_simulation : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_create_simulation(QWidget *parent = 0);
    ~CWE_create_simulation();

private:
    Ui::CWE_create_simulation *ui;
};

#endif // CWE_CREATE_SIMULATION_H
