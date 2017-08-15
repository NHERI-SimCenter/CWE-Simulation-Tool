#ifndef CWE_WELCOME_SCREEN_H
#define CWE_WELCOME_SCREEN_H

#include <QFrame>

namespace Ui {
class CWE_welcome_screen;
}

class CWE_welcome_screen : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_welcome_screen(QWidget *parent = 0);
    ~CWE_welcome_screen();

private:
    Ui::CWE_welcome_screen *ui;
};

#endif // CWE_WELCOME_SCREEN_H
