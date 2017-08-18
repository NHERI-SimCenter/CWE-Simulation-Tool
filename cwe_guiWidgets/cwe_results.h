#ifndef CWE_RESULTS_H
#define CWE_RESULTS_H

#include <QWidget>
class MyTableModel;

namespace Ui {
class CWE_Results;
}

class CWE_Results : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Results(QWidget *parent = 0);
    ~CWE_Results();
    void setName(const QString &s);
    void setType(const QString &s);
    void setLocation(const QString &s);
    void addResult(QString, bool, bool, QString, QString, QString);

private:
    Ui::CWE_Results *ui;
    MyTableModel    *model;
    void addDummyResult(void);
};

#endif // CWE_RESULTS_H
