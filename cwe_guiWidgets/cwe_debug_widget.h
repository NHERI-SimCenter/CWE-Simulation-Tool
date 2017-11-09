#ifndef CWE_DEBUG_WIDGET_H
#define CWE_DEBUG_WIDGET_H

#include <QWidget>

namespace Ui {
class CWE_Debug_Widget;
}

class CWE_Debug_Widget : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Debug_Widget(QWidget *parent = 0);
    ~CWE_Debug_Widget();

private:
    Ui::CWE_Debug_Widget *ui;
};

#endif // CWE_DEBUG_WIDGET_H
