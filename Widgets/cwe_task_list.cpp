#include "cwe_task_list.h"
#include "ui_cwe_task_list.h"

CWE_task_list::CWE_task_list(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_task_list)
{
    ui->setupUi(this);
}

CWE_task_list::~CWE_task_list()
{
    delete ui;
}
