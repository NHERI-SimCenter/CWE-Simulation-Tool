#include "copyrightdialog.h"
#include "ui_copyrightdialog.h"

CopyrightDialog::CopyrightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CopyrightDialog)
{
    ui->setupUi(this);
}

CopyrightDialog::~CopyrightDialog()
{
    delete ui;
}
