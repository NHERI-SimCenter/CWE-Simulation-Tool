#ifndef CWE_RESULTS_H
#define CWE_RESULTS_H

#include <QWidget>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QPixmap>

class VWTinterfaceDriver;
enum class CaseState;

namespace Ui {
class CWE_Results;
}

class CWE_Results : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Results(QWidget *parent = 0);
    ~CWE_Results();

    void linkWithDriver(VWTinterfaceDriver * newDriver);
    void resetViewInfo();

private slots:
    void on_downloadEntireCaseButton_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

private:
    Ui::CWE_Results    *ui;
    QStandardItemModel *model;
    VWTinterfaceDriver *myDriver;

    bool viewIsValid = false;

    void addResult(QString name, bool showeye, bool download, QString type, QString description, QString filesize);
};

#endif // CWE_RESULTS_H
