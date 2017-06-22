#include "cwe_mainwindow.h"
#include "ui_cwe_mainwindow.h"

#include <QDebug>

CWE_MainWindow::CWE_MainWindow(VWTinterfaceDriver *newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWE_MainWindow)
{
    int idx;
    QPushButton *btn;

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

    QStackedLayout *stackLayout = new QStackedLayout();
    stackLayout->setContentsMargins(0,0,0,0);

    // 1) create new simulation
    taskLanding          = new CWE_landing(widgetStack);
    stackLayout->addWidget(taskLanding);

    taskCreateSimulation = new CWE_create_simulation(widgetStack);
    stackLayout->addWidget(taskCreateSimulation);
    idx = stackLayout->currentIndex();
    btn = taskSideBar->findChild<QPushButton *>("pb_sideBar_create");

    qDebug() << "2: " << idx << ": " << btn;

    connect(btn, SIGNAL(clicked()), stackLayout, SLOT(setCurrentIndex(idx)));

    // 2) manage and run simulation
    taskManageSimulation = new CWE_manage_simulation(widgetStack);
    stackLayout->addWidget(taskManageSimulation);

    // 3) manage and download remote files
    taskFileManager      = new CWE_file_manager(widgetStack);
    stackLayout->addWidget(taskFileManager);
    idx = stackLayout->currentIndex();
    btn = taskSideBar->findChild<QPushButton *>("pb_sideBar_files");

    qDebug() << "3: " << idx << ": " << btn;

    connect(btn, SIGNAL(clicked()), stackLayout, SLOT(setCurrentIndex(idx)));

    // 4) manage remote job
    taskSimulationDetail = new CWE_simulation_details(widgetStack);
    taskTaskList         = new CWE_task_list(widgetStack);

    stackLayout->addWidget(taskSimulationDetail);
    idx = stackLayout->currentIndex();
    btn = taskSideBar->findChild<QPushButton *>("pb_sideBar_run");

    qDebug() << "4a: " << idx << ": " << btn;

    connect(btn, SIGNAL(clicked()), stackLayout, SLOT(setCurrentIndex(idx)));

    stackLayout->addWidget(taskTaskList);
    idx = stackLayout->currentIndex();
    btn = taskSideBar->findChild<QPushButton *>("pb_sideBar_jobs");

    qDebug() << "4b: " << idx << ": " << btn;

    connect(btn, SIGNAL(clicked()), stackLayout, SLOT(setCurrentIndex(idx)));

    // 5) tutorial and help
    taskHelp             = new CWE_help(widgetStack);
    stackLayout->addWidget(taskHelp);
    idx = stackLayout->currentIndex();
    btn = taskSideBar->findChild<QPushButton *>("pb_sideBar_help");

    qDebug() << "5: " << idx << ": " << btn;

    connect(btn, SIGNAL(clicked()), stackLayout, SLOT(setCurrentIndex(idx)));

    widgetStack->setLayout(stackLayout);
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
