#include "sidebar.h"
#include "ui_sidebar.h"

SideBar::SideBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SideBar)
{
    ui->setupUi(this);
}

SideBar::~SideBar()
{
    delete ui;
}

/* private slots */

void SideBar::pb_sideBar_create_clicked()
{
   emit taskSelected(TASK_CREATE_NEW_SIMULATION);
}

void SideBar::pb_sideBar_files_clicked()
{
   emit taskSelected(TASK_MANAGE_FILES);
}

void SideBar::pb_sideBar_run_clicked()
{
   emit taskSelected(TASK_MANAGE_SIMULATION);
}

void SideBar::pb_sideBar_jobs_clicked()
{
   emit taskSelected(TASK_MANAGE_JOBS);
}

void SideBar::pb_sideBar_help_clicked()
{
   emit taskSelected(TASK_HELP);
}

