#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include "Widgets/cwe_defines.h"

namespace Ui {
class SideBar;
}

class SideBar : public QWidget
{
    Q_OBJECT

public:
    explicit SideBar(QWidget *parent = 0);
    ~SideBar();

private slots:
   void pb_sideBar_create_clicked();
   void pb_sideBar_files_clicked();
   void pb_sideBar_run_clicked();
   void pb_sideBar_jobs_clicked();
   void pb_sideBar_help_clicked();

signals:
    void taskSelected(TASK);

private:
    Ui::SideBar *ui;
};

#endif // SIDEBAR_H
