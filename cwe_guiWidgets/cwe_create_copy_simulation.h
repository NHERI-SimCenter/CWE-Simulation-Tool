#ifndef CWE_CREATE_COPY_SIMULATION_H
#define CWE_CREATE_COPY_SIMULATION_H

#include <QFrame>
#include <QMap>
#include <QList>
#include <QPushButton>
#include <QRadioButton>

#include "cwe_super.h"

class FileOperator;
class VWTinterfaceDriver;
class CFDanalysisType;

struct CASE_TYPE_DATA {
    QRadioButton         *radioBtn;
    QPushButton          *pbtn;
    CFDanalysisType      *templateData;
};

namespace Ui {
class CWE_Create_Copy_Simulation;
}

class CWE_Create_Copy_Simulation : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Create_Copy_Simulation(QWidget *parent = 0);
    ~CWE_Create_Copy_Simulation();

    virtual void linkDriver(VWTinterfaceDriver * theDriver);

private slots:
    void on_pBtn_create_copy_clicked();
    void on_tabWidget_currentChanged(int index);
    void selectCaseTemplate();

private:
    Ui::CWE_Create_Copy_Simulation *ui;
    void populateCaseTypes();
    void create_new_case_from_template(QString filename);

    CFDanalysisType * selectedTemplate = NULL;

    QVector<CASE_TYPE_DATA> caseTypeDataList;
};

#endif // CWE_CREATE_COPY_SIMULATION_H
