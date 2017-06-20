#ifndef CWE05_H
#define CWE05_H

#include <QWidget>

namespace Ui {
class CWE05;
}

class CWE05 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE05(QWidget *parent = 0);
    ~CWE05();

private:
    Ui::CWE05 *ui;
};

#endif // CWE05_H
