#ifndef CWE_MAINWINDOW_H
#define CWE_MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QStackedLayout>
#include <QPushButton>

#include "Widgets/cwe_defines.h"
#include "Widgets/cwe_landing.h"
#include "Widgets/cwe_create_simulation.h"
#include "Widgets/cwe_file_manager.h"
#include "Widgets/cwe_manage_simulation.h"
#include "Widgets/cwe_simulation_details.h"
#include "Widgets/cwe_task_list.h"
#include "Widgets/cwe_help.h"
#include "Widgets/sidebar.h"

#include "taskPanelWindow/taskpanelentry.h"
#include "taskPanelWindow/placeholderpanel.h"
#include "taskPanelWindow/cfdpanel.h"
#include "taskPanelWindow/debugagaveapppanel.h"
#include "taskPanelWindow/simplenamevalpanel.h"
#include "taskPanelWindow/visualpanel.h"

#include "utilWindows/copyrightdialog.h"
#include "fileWindow/remotefilewindow.h"
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
    void taskEntryClicked(QModelIndex clickedItem);
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
    RemoteFileWindow       *fileTreeModel;

    QMap<TASK, int>        stackedWidgetsIndex;
    QStackedLayout         *stackLayout;


    QVector<TaskPanelEntry *> taskPanelList;
    QStandardItemModel taskListModel;
    const QStringList taskHeaderList = {"Task List:","idNum"};

    void registerTaskPanel(TaskPanelEntry * newPanel);
    void takePanelOwnership(TaskPanelEntry * newOwner);
};

#endif // CWE_MAINWINDOW_H
