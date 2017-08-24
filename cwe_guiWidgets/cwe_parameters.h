#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include <QWidget>

namespace Ui {
class CWE_Parameters;
}

class CWE_Parameters : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Parameters(QWidget *parent = 0);
    ~CWE_Parameters();
    void setName(const QString &s);
    void setType(const QString &s);
    void setLocation(const QString &s);
    int  setTemplate(const QString &filename);

private slots:
    void on_pbtn_saveAllParameters_clicked();
    void on_pBtn_simulation_run_clicked();
    void on_pBtn_simulation_cancel_clicked();
    void on_pBtn_simulation_results_clicked();
    void on_pBtn_simulation_rollback_clicked();
    void on_pBtn_model_run_clicked();
    void on_pBtn_model_cancel_clicked();
    void on_pBtn_model_results_clicked();
    void on_pBtn_model_rollback_clicked();

private:
    Ui::CWE_Parameters *ui;
};

#endif // CWE_PARAMETERS_H
