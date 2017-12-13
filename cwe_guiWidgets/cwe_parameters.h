#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include <QWidget>
#include <QMap>
#include <QFile>
//#include <QJsonDocument>
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
class CWE_StageTab;
class QJsonDocument;

enum class CaseState;

enum class CaseCommand { ROLLBACK, RUN, CANCEL };

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
    void initStateTabs();

    void switchToResults();
    void performCaseCommand(QString stage, CaseCommand toEnact);

    void setParameterConfig(QJsonDocument &doc);

private slots:
    void on_pbtn_saveAllParameters_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

private:
    void saveAllParams();

    Ui::CWE_Parameters *ui;

    bool viewIsValid = false;

    QMap<QString, int> stageTabsIndex;

    QJsonDocument *configFile;
};

#endif // CWE_PARAMETERS_H
