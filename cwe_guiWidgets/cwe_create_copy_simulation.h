#ifndef CWE_CREATE_COPY_SIMULATION_H
#define CWE_CREATE_COPY_SIMULATION_H

#include <QFrame>
#include <QMap>
#include <QList>
#include <QPushButton>
#include <QRadioButton>

class FileOperator;
class VWTinterfaceDriver;

//enum class SimulationType { CHANNEL_FLOW, SHAPE_2D, SHAPE_3D };

typedef struct CASE_TYPE_DATA {
    QRadioButton *radioBtn;
    QPushButton  *pbtn;
    QString      caseFile;
};

namespace Ui {
class CWE_Create_Copy_Simulation;
}

class CWE_Create_Copy_Simulation : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_Create_Copy_Simulation(QWidget *parent = 0);
    ~CWE_Create_Copy_Simulation();

    void linkDriver(VWTinterfaceDriver * theDriver);

private slots:
    void on_lineEdit_newCaseName_editingFinished();
    void on_pBtn_cancel_clicked();
    void on_pBtn_create_copy_clicked();
    void on_tabWidget_currentChanged(int index);
    void selectCaseTemplate();

signals:
    void needParamTab();

private:
    Ui::CWE_Create_Copy_Simulation *ui;
    void populateCaseTypes(QStringList &caseTypeFiles);
    void create_new_case_from_template(QString filename);

    //SimulationType setSimulationType(SimulationType);

    //QVector<QList<QWidget *> > *templateListMap;
    QVector<CASE_TYPE_DATA> *caseTypeDataList;
    VWTinterfaceDriver * driverLink = NULL;
};

#endif // CWE_CREATE_COPY_SIMULATION_H
