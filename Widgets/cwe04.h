#ifndef CWE04_H
#define CWE04_H

#include <QWidget>

namespace Ui {
class CWE04;
}

class CWE04 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE04(QWidget *parent = 0);
    ~CWE04();

private:
    Ui::CWE04 *ui;
};

#endif // CWE04_H
