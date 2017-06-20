#ifndef CWE03_H
#define CWE03_H

#include <QWidget>

namespace Ui {
class CWE03;
}

class CWE03 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE03(QWidget *parent = 0);
    ~CWE03();

private:
    Ui::CWE03 *ui;
};

#endif // CWE03_H
