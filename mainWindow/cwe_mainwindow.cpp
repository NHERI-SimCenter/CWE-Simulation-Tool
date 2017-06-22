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
    taskTreeView = this->findChild<QTreeView *>("taskTreeView");
    sharedWidget = this->findChild<QStackedWidget *>("stackedView");

    if (!taskTreeView) {
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
        stackLayout->addWidget(taskLanding);
        stackedWidgetsIndex.insert(TASK_LANDING, stackLayout->currentIndex());

        taskCreateSimulation = new CWE_create_simulation(widgetStack);
        stackLayout->addWidget(taskCreateSimulation);
        stackedWidgetsIndex.insert(TASK_CREATE_NEW_SIMULATION, stackLayout->currentIndex());

        // 2) manage and run simulation
        taskManageSimulation = new CWE_manage_simulation(widgetStack);
        stackLayout->addWidget(taskManageSimulation);
        stackedWidgetsIndex.insert(TASK_MANAGE_SIMULATION, stackLayout->currentIndex());

        // 3) manage and download remote files
        taskFileManager      = new CWE_file_manager(widgetStack);
        stackLayout->addWidget(taskFileManager);
        stackedWidgetsIndex.insert(TASK_MANAGE_FILES, stackLayout->currentIndex());

        // 4) manage remote job
        taskSimulationDetail = new CWE_simulation_details(widgetStack);
        stackLayout->addWidget(taskSimulationDetail);
        stackedWidgetsIndex.insert(TASK_MANAGE_SIMULATION, stackLayout->currentIndex());

        taskTaskList         = new CWE_task_list(widgetStack);
        stackLayout->addWidget(taskTaskList);
        stackedWidgetsIndex.insert(TASK_LIST_TASKS, stackLayout->currentIndex());

        // 5) tutorial and help
        taskHelp             = new CWE_help(widgetStack);
        stackLayout->addWidget(taskHelp);
        stackedWidgetsIndex.insert(TASK_HELP, stackLayout->currentIndex());

        widgetStack->setLayout(stackLayout);

        connect(taskSideBar, SIGNAL(taskSelected(TASK)), this, SLOT(SideBar_task_selected(TASK)));
    }
    else
    {
        taskTreeView->setModel(&taskListModel);
        taskListModel.setHorizontalHeaderLabels(taskHeaderList);
        taskTreeView->hideColumn(1);
    }
    fileTreeModel = myDriver->getFileDisplay();
}

CWE_MainWindow::~CWE_MainWindow()
{
    delete ui;
    if (fileTreeModel) delete fileTreeModel;
    if (taskCreateSimulation) delete taskCreateSimulation;
    if (taskFileManager) delete taskFileManager;
    if (taskLanding) delete taskLanding;
    if (taskManageSimulation) delete taskManageSimulation;
    if (taskSimulationDetail) delete taskSimulationDetail;
    if (taskTaskList) delete taskTaskList;
    if (taskHelp) delete taskHelp;
}

void CWE_MainWindow::setupTaskList()
{
    //Populate panel list:
    TaskPanelEntry * realPanel;
    PlaceholderPanel * aPanel;

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Create Simulation", ". . . Empty Channel Flow"});
    registerTaskPanel(aPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Create Simulation", ". . . Standard Shapes"});
    registerTaskPanel(aPanel);

    realPanel = new SimpleNameValPanel(dataLink, myDriver->getFileDisplay(),
    {"Create Simulation", ". . . From Geometry File (2D slice)"},
    {"turbModel", "nu", "velocity", "endTime", "deltaT", "B", "H", "pisoCorrectors", "pisoNonOrthCorrect"},
    {"SlicePlane", "NewCaseFolder"}, "SimParams" ,"twoDslice");
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Create Simulation", ". . . From Geometry File (3D)"});
    registerTaskPanel(aPanel);

    realPanel = new SimpleNameValPanel(dataLink, myDriver->getFileDisplay(),
    {"Mesh Generation", ". . . From Simple Geometry Format"},
    {"boundaryTop", "boundaryLow", "inPad", "outPad", "topPad", "bottomPad", "meshDensity", "meshDensityFar"},
    {}, "MeshParams" ,"twoDUmesh");
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Mesh Generation", ". . . For Empty Channel"});
    registerTaskPanel(aPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Mesh Generation", ". . . Using Shape Template"});
    registerTaskPanel(aPanel);

    realPanel = new CFDpanel(dataLink, fileTreeModel);
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Run/Setup Simulation", ". . . Modify Turbulence Parameters"});
    registerTaskPanel(aPanel);
    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Run/Setup Simulation", ". . . Modify Inflow Parameters"});
    registerTaskPanel(aPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Post-Process", "Extract Data"});
    registerTaskPanel(aPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"View Results", "Quick Simulation Stats"});
    registerTaskPanel(aPanel);

    realPanel = new VisualPanel(dataLink, fileTreeModel,{"View Results", "Visualize Mesh"},NULL);
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"View Results", "Visualize Velocity/Pressure Fields"});
    registerTaskPanel(aPanel);
    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"View Results", "Create Data Graphs"});
    registerTaskPanel(aPanel);

    realPanel = new DebugAgaveAppPanel(dataLink, fileTreeModel);
    registerTaskPanel(realPanel);

    //We then activate the first entry in the list
    taskEntryClicked(taskListModel.invisibleRootItem()->child(0,0)->index());
}


void CWE_MainWindow::registerTaskPanel(TaskPanelEntry * newPanel)
{
    //TODO: Probably should use style sheets for this coloring
    QColor greyColor(150,150,150);
    QBrush greyFont(greyColor);

    QColor blackColor(0,0,0);
    QBrush blackFont(blackColor);

    taskPanelList.append(newPanel);

    QWidget * newWidgetPane = newPanel->getOwnedWidget();
    sharedWidget->addWidget(newWidgetPane);

    //TODO: Double check that id and index numbers match each other

    if (taskPanelList.length() == 1)
    {
        takePanelOwnership(newPanel);
    }

    QStringList nameList = newPanel->getFrameNames();

    QStandardItem * searchNode = taskListModel.invisibleRootItem();
    for (auto itr = nameList.cbegin(); itr != nameList.cend(); itr++)
    {
        bool newEntry = true;
        for (int i = 0; i < searchNode->rowCount(); i++ )
        {
            if (searchNode->child(i)->text() == (*itr))
            {
                newEntry = false;
                searchNode = searchNode->child(i);
                i = searchNode->rowCount();
            }
        }
        if (newEntry)
        {
            QList<QStandardItem*> newItemRow;
            QStandardItem * nameEntry = new QStandardItem(*itr);

            nameEntry->setForeground(greyFont);

            newItemRow.append(nameEntry);
            newItemRow.append(new QStandardItem("-1"));
            searchNode->appendRow(newItemRow);
            searchNode = searchNode->child(searchNode->rowCount() - 1);
        }
        if (newPanel->isImplemented())
        {
            searchNode->setForeground(blackFont);
        }
    }
    int currRow = searchNode->row();
    searchNode->parent()->child(currRow,1)->setText(QString::number(newPanel->getFrameId()));
}

void CWE_MainWindow::takePanelOwnership(TaskPanelEntry * newOwner)
{
    if (newOwner->isCurrentActiveFrame())
    {
        //This is already active frame, so do nothing.
        return;
    }
    sharedWidget->setCurrentIndex(sharedWidget->indexOf(newOwner->getOwnedWidget()));
    if ((TaskPanelEntry::getActiveFrameId() >= 0) && (TaskPanelEntry::getActiveFrameId() < taskPanelList.length()))
    {
        TaskPanelEntry * oldOwner = taskPanelList.at(TaskPanelEntry::getActiveFrameId());
        oldOwner->frameNowInvisible();
    }

    newOwner->setAsActive();

    newOwner->frameNowVisible();
}

void CWE_MainWindow::menuCopyInfo()
{
    CopyrightDialog copyrightPopup;
    copyrightPopup.exec();
}

void CWE_MainWindow::taskEntryClicked(QModelIndex clickedItem)
{
    //TODO: Figure out proper behavior if click down and release up are on two different items
    QStandardItem * clickedTextEntry = taskListModel.itemFromIndex(clickedItem);
    int entryRow = clickedTextEntry->row();
    int taskIndex = -1;
    if (clickedTextEntry->parent() == NULL)
    {
        taskIndex = taskListModel.invisibleRootItem()->child(entryRow, 1)->text().toInt();
    }
    else
    {
        taskIndex = clickedTextEntry->parent()->child(entryRow, 1)->text().toInt();
    }
    if (taskIndex == -1)
    {
        //We need to expand an try again one level down
        if (clickedTextEntry->hasChildren())
        {
            taskTreeView->expand(clickedItem);
            taskTreeView->selectionModel()->select(clickedTextEntry->child(0,0)->index(),QItemSelectionModel::ClearAndSelect);
            taskEntryClicked(clickedTextEntry->child(0,0)->index());
        }
        return;
    }
    //Otherwise, give control to that panel
    takePanelOwnership(taskPanelList.at(taskIndex));
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
