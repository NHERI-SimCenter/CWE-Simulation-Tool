#ifndef CWE02_H
#define CWE02_H

#include <QWidget>

namespace Ui {
class CWE02;
}

class CWE02 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE02(QWidget *parent = 0);
    ~CWE02();

private:
    Ui::CWE02 *ui;
};

#endif // CWE02_H
