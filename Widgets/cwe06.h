#ifndef CWE06_H
#define CWE06_H

#include <QWidget>

namespace Ui {
class CWE06;
}

class CWE06 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE06(QWidget *parent = 0);
    ~CWE06();

private:
    Ui::CWE06 *ui;
};

#endif // CWE06_H
