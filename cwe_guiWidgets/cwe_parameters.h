#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include <QWidget>
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QLineEdit>

#include "cwe_super.h"

class CFDcaseInstance;
class CFDanalysisType;
class VWTinterfaceDriver;

enum class CaseState;

namespace Ui {
class CWE_Parameters;
}

class CWE_Parameters : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Parameters(QWidget *parent = 0);
    ~CWE_Parameters();

    virtual void linkDriver(VWTinterfaceDriver * newDriver);
    void resetViewInfo();

private slots:
    void on_pbtn_saveAllParameters_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

    void switchToResultsSlot();
    void switchToParameterSlot();
    void switchToCreateSlot();

private:
    void saveAllParams();

    Ui::CWE_Parameters *ui;

    bool viewIsValid = false;

    QMap<QString, int> parameterTabs;
};

#endif // CWE_PARAMETERS_H
