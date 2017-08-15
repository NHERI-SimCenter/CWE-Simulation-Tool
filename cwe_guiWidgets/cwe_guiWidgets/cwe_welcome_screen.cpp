#include "cwe_guiWidgets/cwe_welcome_screen.h"
#include "ui_cwe_welcome_screen.h"

CWE_welcome_screen::CWE_welcome_screen(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_welcome_screen)
{
    ui->setupUi(this);
}

CWE_welcome_screen::~CWE_welcome_screen()
{
    delete ui;
}
