#ifndef CWE01_H
#define CWE01_H

#include <QWidget>

namespace Ui {
class CWE01;
}

class CWE01 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE01(QWidget *parent = 0);
    ~CWE01();

private:
    Ui::CWE01 *ui;
};

#endif // CWE01_H
