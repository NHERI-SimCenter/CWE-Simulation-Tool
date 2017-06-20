#ifndef CWE04a_H
#define CWE04a_H

#include <QWidget>

namespace Ui {
class CWE04a;
}

class CWE04a : public QWidget
{
    Q_OBJECT

public:
    explicit CWE04a(QWidget *parent = 0);
    ~CWE04a();

private:
    Ui::CWE04a *ui;
};

#endif // CWE04a_H
