/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "panelwindow.h"
#include "ui_panelwindow.h"

#include "agaveInterfaces/filetreemodelreader.h"
#include "taskPanels/taskpanelentry.h"
#include "taskPanels/filemainippanel.h"
#include "taskPanels/placeholderpanel.h"
#include "../vwtinterfacedriver.h"

PanelWindow::PanelWindow(VWTinterfaceDriver *newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PanelWindow)
{
    ui->setupUi(this);

    interfaceDriver = newDriver;

    taskTreeView = this->findChild<QTreeView *>("taskTreeView");
    sharedWidget = this->findChild<QStackedWidget *>("stackedView");
    QTreeView * fileTreeView = this->findChild<QTreeView *>("fileTreeView");
    QLabel * fileLabel = this->findChild<QLabel *>("filesLabel");

    taskTreeView->setModel(&taskListModel);
    taskListModel.setHorizontalHeaderLabels(taskHeaderList);
    taskTreeView->hideColumn(1);

    fileTreeModel = new FileTreeModelReader(interfaceDriver, fileTreeView, fileLabel);
}

PanelWindow::~PanelWindow()
{
    delete ui;
    delete fileTreeModel;
}

void PanelWindow::setupTaskList()
{
    //Start first query for files:
    fileTreeModel->resetFileData();

    //Populate panel list:
    FileMainipPanel * filePanel = new FileMainipPanel(interfaceDriver, fileTreeModel);
    registerTaskPanel(filePanel);

    PlaceholderPanel * placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Create Simulation", ". . . Empty Channel Flow"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Create Simulation", ". . . Standard Shapes"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Create Simulation", ". . . From Geometry File (2D slice)"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Create Simulation", ". . . From Geometry File (3D)"});
    registerTaskPanel(placeHolderEntry);

    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Mesh Generation", ". . . From Simple Geometry Format"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Mesh Generation", ". . . For Empty Channel"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Mesh Generation", ". . . Using Shape Template"});
    registerTaskPanel(placeHolderEntry);

    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Run/Setup Simulation", ". . . Using OpenFOAM"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Run/Setup Simulation", ". . . Modify Turbulence Parameters"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Run/Setup Simulation", ". . . Modify Inflow Parameters"});
    registerTaskPanel(placeHolderEntry);

    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"Post-Process", "Extract Data"});
    registerTaskPanel(placeHolderEntry);

    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"View Results", "Quick Simulation Stats"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"View Results", "Visualize Mesh/Geometry"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"View Results", "Visualize Velocity/Pressure Fields"});
    registerTaskPanel(placeHolderEntry);
    placeHolderEntry = new PlaceholderPanel();
    placeHolderEntry->setPlaceHolderText({"View Results", "Create Data Graphs"});
    registerTaskPanel(placeHolderEntry);

    //We then activate the first entry in the list
    taskEntryClicked(taskListModel.invisibleRootItem()->child(0,0)->index());
}


void PanelWindow::registerTaskPanel(TaskPanelEntry * newPanel)
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

void PanelWindow::takePanelOwnership(TaskPanelEntry * newOwner)
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
    fileTreeModel->setTreeViewVisibility(newOwner->fileTreeIsVisible());

    newOwner->frameNowVisible();
}

void PanelWindow::menuCopyInfo()
{
    interfaceDriver->displayCopyInfo();
}

void PanelWindow::taskEntryClicked(QModelIndex clickedItem)
{
    //TODO: Figure out proper behavior if click down and release up arre on two different items
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

void PanelWindow::menuExit()
{
    qApp->exit(0);
}
