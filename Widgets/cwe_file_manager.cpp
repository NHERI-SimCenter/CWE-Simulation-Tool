#include "cwe_file_manager.h"
#include "ui_cwe_file_manager.h"

CWE_file_manager::CWE_file_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_file_manager)
{
    ui->setupUi(this);
    QPixmap lpix(":/buttons/images/CWE_button_left_arrows.png");
    ui->lbl_left_arrows->setPixmap(lpix);
    QPixmap rpix(":/buttons/images/CWE_button_right_arrows.png");
    ui->lbl_right_arrows->setPixmap(rpix);
}

CWE_file_manager::~CWE_file_manager()
{
    delete ui;
}
