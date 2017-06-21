#include "cwe_mainwindow.h"
#include "ui_cwe_mainwindow.h"

CWE_MainWindow::CWE_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWE_MainWindow)
{
    ui->setupUi(this);
}

CWE_MainWindow::~CWE_MainWindow()
{
    delete ui;
}
