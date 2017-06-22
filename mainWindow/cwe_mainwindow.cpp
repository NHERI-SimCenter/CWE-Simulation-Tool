#include "cwe_mainwindow.h"
#include "ui_cwe_mainwindow.h"

#include <QDebug>

CWE_MainWindow::CWE_MainWindow(VWTinterfaceDriver *newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWE_MainWindow)
{
    ui->setupUi(this);

    myDriver = newDriver;
    dataLink = myDriver->getDataConnection();

    /* add the sidebar */
    QWidget *frameSideBar = this->findChild<QWidget *>("SideBar");
    taskSideBar = new SideBar(frameSideBar);
    QGridLayout *layout = new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(taskSideBar);
    frameSideBar->setLayout(layout);

    /* populate the stacked widget */

    // get the stacked widget container
    widgetStack = this->findChild<QWidget *>("stackContainer");

    stackLayout = new QStackedLayout();
    stackLayout->setContentsMargins(0,0,0,0);

    // 1) create new simulation
    taskLanding          = new CWE_landing(widgetStack);
    stackedWidgetsIndex.insert(TASK_LANDING, stackLayout->addWidget(taskLanding));

    taskCreateSimulation = new CWE_create_simulation(widgetStack);
    stackedWidgetsIndex.insert(TASK_CREATE_NEW_SIMULATION, stackLayout->addWidget(taskCreateSimulation));

    // 2) manage and run simulation
    taskManageSimulation = new CWE_manage_simulation(widgetStack);
    stackedWidgetsIndex.insert(TASK_MANAGE_SIMULATION, stackLayout->addWidget(taskManageSimulation));

    // 3) manage and download remote files
    taskFileManager      = new CWE_file_manager(widgetStack);
    stackedWidgetsIndex.insert(TASK_MANAGE_FILES, stackLayout->addWidget(taskFileManager));

    // 4) manage remote job
    taskSimulationDetail = new CWE_simulation_details(widgetStack);
    stackedWidgetsIndex.insert(TASK_MANAGE_SIMULATION, stackLayout->addWidget(taskSimulationDetail));

    taskTaskList         = new CWE_task_list(widgetStack);
    stackedWidgetsIndex.insert(TASK_LIST_TASKS, stackLayout->addWidget(taskTaskList));

    // 5) tutorial and help
    taskHelp             = new CWE_help(widgetStack);
    stackedWidgetsIndex.insert(TASK_HELP, stackLayout->addWidget(taskHelp));

    widgetStack->setLayout(stackLayout);

    connect(taskSideBar, SIGNAL(taskSelected(TASK)), this, SLOT(SideBar_task_selected(TASK)));
}

CWE_MainWindow::~CWE_MainWindow()
{
    delete ui;
    if (taskCreateSimulation) delete taskCreateSimulation;
    if (taskFileManager) delete taskFileManager;
    if (taskLanding) delete taskLanding;
    if (taskManageSimulation) delete taskManageSimulation;
    if (taskSimulationDetail) delete taskSimulationDetail;
    if (taskTaskList) delete taskTaskList;
    if (taskHelp) delete taskHelp;
}

void CWE_MainWindow::menuCopyInfo()
{
    CopyrightDialog copyrightPopup;
    copyrightPopup.exec();
}

void CWE_MainWindow::menuExit()
{
    myDriver->shutdown();
}

void CWE_MainWindow::on_action_Quit_triggered()
{
    myDriver->shutdown();
}

/* side bar functionality */
void CWE_MainWindow::SideBar_task_selected(TASK task)
{
    stackLayout->setCurrentIndex(stackedWidgetsIndex.value(task));
}

void CWE_MainWindow::selectLanding()
{
}

void CWE_MainWindow::selectCreateSimulation()
{
}

void CWE_MainWindow::selectManageRun()
{
}

void CWE_MainWindow::selectManageJobs()
{
}

void CWE_MainWindow::selectManageFiles()
{
}

void CWE_MainWindow::selectHelp()
{
}
