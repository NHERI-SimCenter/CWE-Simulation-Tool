#ifndef CWE07_H
#define CWE07_H

#include <QWidget>

namespace Ui {
class CWE07;
}

class CWE07 : public QWidget
{
    Q_OBJECT

public:
    explicit CWE07(QWidget *parent = 0);
    ~CWE07();

private:
    Ui::CWE07 *ui;
};

#endif // CWE07_H
