#ifndef CWE_MAINWINDOW_H
#define CWE_MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QStackedLayout>
#include <QPushButton>

#include "Widgets/cwe_landing.h"
#include "Widgets/cwe_create_simulation.h"
#include "Widgets/cwe_file_manager.h"
#include "Widgets/cwe_manage_simulation.h"
#include "Widgets/cwe_simulation_details.h"
#include "Widgets/cwe_task_list.h"
#include "Widgets/cwe_help.h"
#include "Widgets/sidebar.h"

#include "utilWindows/copyrightdialog.h"
#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"

/* define an enum for all tasks/widgets stacked on the right side of cwe_mainwindow */
/* note: TASK_MAX_TASKS must always be the last entry.                              *
 *       It will be used for dimensioning a storage array                           */
enum TASK {
    TASK_LANDING,
    TASK_CREATE_NEW_SIMULATION,
    TASK_MANAGE_SIMULATION,
    TASK_MANAGE_FILES,
    TASK_MANAGE_JOBS,
    TASK_HELP,
    TASK_MAX_TASKS
};

namespace Ui {
class CWE_MainWindow;
}

class CWE_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CWE_MainWindow(VWTinterfaceDriver *newDriver, QWidget *parent = 0);
    ~CWE_MainWindow();

    void setupTaskList();

private slots:
    void menuExit();
    void menuCopyInfo();

    void on_action_Quit_triggered();

    /* side bar functionality */
    void selectLanding();
    void selectCreateSimulation();
    void selectManageRun();
    void selectManageJobs();
    void selectManageFiles();
    void selectHelp();

private:
    Ui::CWE_MainWindow *ui;

    VWTinterfaceDriver     *myDriver;

    SideBar                *taskSideBar;
    QWidget                *widgetStack;

    CWE_create_simulation  *taskCreateSimulation;
    CWE_file_manager       *taskFileManager;
    CWE_landing            *taskLanding;
    CWE_manage_simulation  *taskManageSimulation;
    CWE_simulation_details *taskSimulationDetail;
    CWE_task_list          *taskTaskList;
    CWE_help               *taskHelp;

    RemoteDataInterface *dataLink;
};

#endif // CWE_MAINWINDOW_H
