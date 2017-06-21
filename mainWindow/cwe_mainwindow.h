#ifndef CWE_MAINWINDOW_H
#define CWE_MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class CWE_MainWindow;
}

class CWE_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CWE_MainWindow(QWidget *parent = 0);
    ~CWE_MainWindow();

private:
    Ui::CWE_MainWindow *ui;
};

#endif // CWE_MAINWINDOW_H
