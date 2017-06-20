#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>

namespace Ui {
class SideBar;
}

class SideBar : public QWidget
{
    Q_OBJECT

public:
    explicit SideBar(QWidget *parent = 0);
    ~SideBar();

private:
    Ui::SideBar *ui;
};

#endif // SIDEBAR_H
