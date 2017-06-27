#ifndef CWE_MAINWINDOW_H
#define CWE_MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QStackedLayout>
#include <QPushButton>

#include "cwe_guiWidgets/cwe_defines.h"
#include "cwe_guiWidgets/cwe_landing.h"
#include "cwe_guiWidgets/cwe_create_simulation.h"
#include "cwe_guiWidgets/cwe_file_manager.h"
#include "cwe_guiWidgets/cwe_manage_simulation.h"
#include "cwe_guiWidgets/cwe_simulation_details.h"
#include "cwe_guiWidgets/cwe_task_list.h"
#include "cwe_guiWidgets/cwe_help.h"
#include "cwe_guiWidgets/sidebar.h"

#include "utilWindows/copyrightdialog.h"
#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"

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
    void SideBar_task_selected(TASK);
    void selectLanding();
    void selectCreateSimulation();
    void selectManageRun();
    void selectManageJobs();
    void selectManageFiles();
    void selectHelp();

    void on_actionCreate_New_Simulation_triggered();
    void on_actionManage_Simulation_triggered();
    void on_actionOpen_triggered();
    void on_actionOpen_existing_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionAbout_CWE_triggered();
    void on_actionHelp_triggered();
    void on_action_Landing_Page_triggered();
    void on_actionManage_Remote_Jobs_triggered();
    void on_actionTutorials_and_Help_triggered();

    void on_actionManage_and_Download_Files_triggered();

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

    QTreeView              *taskTreeView;
    QStackedWidget         *sharedWidget;
    RemoteDataInterface    *dataLink;

    QMap<TASK, int>        stackedWidgetsIndex;
    QStackedLayout         *stackLayout;
};

#endif // CWE_MAINWINDOW_H
