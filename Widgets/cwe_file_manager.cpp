#include "cwe_file_manager.h"
#include "ui_cwe_file_manager.h"

CWE_file_manager::CWE_file_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_file_manager)
{
    ui->setupUi(this);
}

CWE_file_manager::~CWE_file_manager()
{
    delete ui;
}
