#ifndef CWE_RESULTS_H
#define CWE_RESULTS_H

#include <QWidget>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QPixmap>

#include "cwe_super.h"

class VWTinterfaceDriver;
enum class CaseState;

namespace Ui {
class CWE_Results;
}

class CWE_Results : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Results(QWidget *parent = 0);
    ~CWE_Results();

    virtual void linkDriver(VWTinterfaceDriver * newDriver);
    void resetViewInfo();

private slots:
    void on_downloadEntireCaseButton_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

private:
    Ui::CWE_Results    *ui;
    QStandardItemModel *model;

    bool viewIsValid = false;

    void addResult(QString name, bool showeye, bool download, QString type, QString description, QString filesize);
};

#endif // CWE_RESULTS_H
