#ifndef CWE_CREATE_COPY_SIMULATION_H
#define CWE_CREATE_COPY_SIMULATION_H

#include <QFrame>

enum class SimulationType { CHANNEL_FLOW, SHAPE_2D, SHAPE_3D };

namespace Ui {
class CWE_Create_Copy_Simulation;
}

class CWE_Create_Copy_Simulation : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_Create_Copy_Simulation(QWidget *parent = 0);
    ~CWE_Create_Copy_Simulation();

private slots:
    void on_lineEdit_newCaseName_editingFinished();

    void on_pBtn_cancel_clicked();

    void on_pBtn_create_copy_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_radioButton_2Dshape_clicked();

    void on_radioButton_3Dshape_clicked();

    void on_radioButton_channelFlow_clicked();

private:
    Ui::CWE_Create_Copy_Simulation *ui;
    SimulationType setSimulationType(SimulationType);

};

#endif // CWE_CREATE_COPY_SIMULATION_H
