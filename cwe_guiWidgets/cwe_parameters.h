#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include <QWidget>
#include <QMap>

class CFDagaveApps;
class CFDanalysisType;
class VWTinterfaceDriver;

namespace Ui {
class CWE_Parameters;
}

class CWE_Parameters : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Parameters(QWidget *parent = 0);
    ~CWE_Parameters();

    void linkWithDriver(VWTinterfaceDriver * newDriver);
    void resetViewInfo();

private slots:
    void on_pbtn_saveAllParameters_clicked();

private:
    void saveAllParams();

    Ui::CWE_Parameters *ui;

    bool viewIsValid = false;
    VWTinterfaceDriver * myDriver;

    QMap<QString, int> parameterTabs;
};

#endif // CWE_PARAMETERS_H
