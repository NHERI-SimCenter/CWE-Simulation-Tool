#ifndef CWE_BUSYINDICATOR_H
#define CWE_BUSYINDICATOR_H

#include <QFrame>

namespace Ui {
class CWE_BusyIndicator;
}

class CWE_BusyIndicator : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_BusyIndicator(QWidget *parent = 0);
    ~CWE_BusyIndicator();

private:
    Ui::CWE_BusyIndicator *ui;
};

#endif // CWE_BUSYINDICATOR_H
