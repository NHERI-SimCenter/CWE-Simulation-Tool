#ifndef CWE_DEBUG_WIDGET_H
#define CWE_DEBUG_WIDGET_H

#include <QWidget>
#include "cwe_busyindicator.h"

namespace Ui {
class CWE_Debug_Widget;
}

class CWE_Debug_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Debug_Widget(QWidget *parent = 0);
    ~CWE_Debug_Widget();

private slots:
    void on_pb_setBusy_clicked();
    void on_pb_stopBusy_clicked();

    void on_pb_timedBusy_clicked();

private:
    Ui::CWE_Debug_Widget *ui;
    CWE_BusyIndicator *theBusy;
};

#endif // CWE_DEBUG_WIDGET_H
