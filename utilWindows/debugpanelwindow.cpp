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

#include "debugpanelwindow.h"
#include "ui_debugpanelwindow.h"

#include "utilWindows/copyrightdialog.h"
#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"

#include "remoteFileOps/remotefiletree.h"

DebugPanelWindow::DebugPanelWindow(VWTinterfaceDriver * newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DebugPanelWindow)
{
    ui->setupUi(this);

    myDriver = newDriver;
    dataLink = myDriver->getDataConnection();
    taskTreeView = ui->longTaskView;
    //sharedWidget = this->findChild<QStackedWidget *>("stackedView");

    taskTreeView->setModel(&taskListModel);
    taskListModel.setHorizontalHeaderLabels(taskHeaderList);
    taskTreeView->hideColumn(1);

    QTreeView * remoteFileView = this->findChild<QTreeView *>("remoteFileView");
    QLabel * selectedFileInfo = this->findChild<QLabel *>("selectedFileInfo");

    fileTreeData = new RemoteFileTree(dataLink, remoteFileView, selectedFileInfo, this);
}

DebugPanelWindow::~DebugPanelWindow()
{
    delete ui;
}

void DebugPanelWindow::setupTaskList()
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

    realPanel = new SimpleNameValPanel(dataLink, fileTreeData,
    {"Create Simulation", ". . . From Geometry File (2D slice)"},
    {"turbModel", "nu", "velocity", "endTime", "deltaT", "B", "H", "pisoCorrectors", "pisoNonOrthCorrect"},
    {"SlicePlane", "NewCaseFolder"}, "SimParams" ,"twoDslice");
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"Create Simulation", ". . . From Geometry File (3D)"});
    registerTaskPanel(aPanel);

    realPanel = new SimpleNameValPanel(dataLink, fileTreeData,
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

    realPanel = new CFDpanel(dataLink, fileTreeData);
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

    realPanel = new VisualPanel(dataLink, fileTreeData,{"View Results", "Visualize Mesh"},NULL);
    registerTaskPanel(realPanel);

    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"View Results", "Visualize Velocity/Pressure Fields"});
    registerTaskPanel(aPanel);
    aPanel = new PlaceholderPanel();
    aPanel->setPlaceHolderText({"View Results", "Create Data Graphs"});
    registerTaskPanel(aPanel);

    realPanel = new DebugAgaveAppPanel(dataLink, fileTreeData);
    registerTaskPanel(realPanel);

    //We then activate the first entry in the list
    taskEntryClicked(taskListModel.invisibleRootItem()->child(0,0)->index());
}


void DebugPanelWindow::registerTaskPanel(TaskPanelEntry * newPanel)
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

void DebugPanelWindow::takePanelOwnership(TaskPanelEntry * newOwner)
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

void DebugPanelWindow::taskEntryClicked(QModelIndex clickedItem)
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

void VisualPanel::selectedFileChanged(FileMetaData * newSelection)
{
    conditionalPurge(&pointData);
    conditionalPurge(&faceData);
    conditionalPurge(&ownerData);
    myCanvas->setDisplayState(CFDDisplayState::TEST_BOX);

    if (newSelection->getFileType() == FileType::DIR)
    {
        FileTreeNode * currentNode = myTreeReader->getFileNodeFromPath(newSelection->getFullPath());

        FileTreeNode * constantFolder = currentNode->getChildNodeWithName("constant");
        if (constantFolder == NULL) return;
        FileTreeNode * polyMeshFolder = constantFolder->getChildNodeWithName("polyMesh");
        if (polyMeshFolder == NULL) return;
        FileTreeNode * pointsFile = polyMeshFolder->getChildNodeWithName("points");
        FileTreeNode * facesFile = polyMeshFolder->getChildNodeWithName("faces");
        FileTreeNode * ownerFile = polyMeshFolder->getChildNodeWithName("owner");
        if (pointsFile == NULL) pointsFile = polyMeshFolder->getChildNodeWithName("points.gz");
        if (facesFile == NULL) facesFile = polyMeshFolder->getChildNodeWithName("faces.gz");
        if (ownerFile == NULL) ownerFile = polyMeshFolder->getChildNodeWithName("owner.gz");

        if ((pointsFile == NULL) || (facesFile == NULL) || (ownerFile == NULL)) return;

        RemoteDataReply * aReply = dataConnection->downloadBuffer(pointsFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

        aReply = dataConnection->downloadBuffer(facesFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));

        aReply = dataConnection->downloadBuffer(ownerFile->getFileData().getFullPath());
        QObject::connect(aReply,SIGNAL(haveBufferDownloadReply(RequestState,QByteArray*)),
                         this,SLOT(gotNewRawFile(RequestState,QByteArray*)));
    }
}

void VisualPanel::conditionalPurge(QByteArray ** theArray)
{
    if (*theArray == NULL) return;
    delete *theArray;
    *theArray = NULL;
}

void VisualPanel::gotNewRawFile(RequestState authReply, QByteArray * fileBuffer)
{
    if (authReply != RequestState::GOOD) return;

    RemoteDataReply * mySender = (RemoteDataReply *) QObject::sender();
    if (mySender == NULL) return;
    QString lookedForFile = mySender->getTaskParamList()->value("remoteName");
    if (lookedForFile.isEmpty()) return;

    QByteArray * realContents;

    if (lookedForFile.endsWith(".gz"))
    {
        lookedForFile.chop(3);
        DeCompressWrapper decompresser(fileBuffer);
        realContents = decompresser.getDecompressedFile();
    }
    else
    {
        realContents = new QByteArray(*fileBuffer);
    }
    if (lookedForFile.endsWith("points"))
    {
        conditionalPurge(&pointData);
        pointData = realContents;
    }
    if (lookedForFile.endsWith("faces"))
    {
        conditionalPurge(&faceData);
        faceData = realContents;
    }
    if (lookedForFile.endsWith("owner"))
    {
        conditionalPurge(&ownerData);
        ownerData = realContents;
    }
    if ((pointData != NULL) && (faceData != NULL) && (ownerData != NULL))
    {
        if(myCanvas->loadMeshData(pointData, faceData, ownerData))
        {
            myCanvas->setDisplayState(CFDDisplayState::MESH);
        }
    }
}

void CFDpanel::cfdSelected()
{
    qDebug("Beginning CFD task");
    QMultiMap<QString, QString> oneInput;
    oneInput.insert("solver","pisoFoam");
    FileMetaData fileData = myTreeReader->getCurrentSelectedFile();
    if (fileData.getFileType() != FileType::DIR)
    {
        //TODO: give reasonable error
        return;
    }
    RemoteDataReply * compressTask = dataConnection->runRemoteJob("openfoam",oneInput,fileData.getFullPath());
    if (compressTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(compressTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(finishedCFDinvoke(RequestState,QJsonDocument*)));
}

void CFDpanel::finishedCFDinvoke(RequestState finalState, QJsonDocument *)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

DebugAgaveAppPanel::DebugAgaveAppPanel(RemoteDataInterface * newDataHandle, RemoteFileTree * newReader, QObject *parent) : TaskPanelEntry(parent)
{
    this->setFrameNameList({"Debug", "Test Agave App"});

    myTreeReader = newReader;
    dataConnection = newDataHandle;

    agaveAppList.appendRow(new QStandardItem("FileEcho"));
    inputLists.insert("FileEcho", {"NewFile", "EchoText"});
    agaveAppList.appendRow(new QStandardItem("PythonTest"));
    inputLists.insert("PythonTest", {"NewFile"});
    agaveAppList.appendRow(new QStandardItem("SectionMesh"));
    inputLists.insert("SectionMesh", {"directory", "SlicePlane","SimParams"});
}

void DebugAgaveAppPanel::setupOwnFrame()
{
    vLayout = new QVBoxLayout;

    agaveOptionList = new QListView();
    agaveOptionList->setModel(&agaveAppList);
    agaveOptionList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QObject::connect(agaveOptionList,SIGNAL(clicked(QModelIndex)), this, SLOT(placeInputPairs(QModelIndex)));
    vLayout->addWidget(agaveOptionList);

    startButton = new QPushButton("Start Agave App");
    QObject::connect(startButton,SIGNAL(clicked(bool)),this,SLOT(commandInvoked()));
    vLayout->addWidget(startButton);

    getOwnedWidget()->setLayout(vLayout);
}

void DebugAgaveAppPanel::placeInputPairs(QModelIndex newSelected)
{
    if (buttonArea != NULL)
    {
        QLayoutItem * toDelete;
        toDelete = buttonArea->takeAt(0);
        while (toDelete != NULL)
        {
            delete toDelete->widget();
            delete toDelete;
            toDelete = buttonArea->takeAt(0);
        }
        delete buttonArea;
        buttonArea = NULL;
    }

    QString selectedApp = agaveAppList.itemFromIndex(newSelected)->text();
    if (selectedApp.isEmpty())
    {
        return;
    }

    QStringList inputList = inputLists.value(selectedApp);

    if (inputList.size() <= 0)
    {
        return;
    }

    buttonArea = new QGridLayout();
    int rowNum = 0;

    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QLabel * tmpLabel = new QLabel(*itr);
        QLineEdit * tmpInput = new QLineEdit();
        tmpInput->setObjectName(*itr);

        buttonArea->addWidget(tmpLabel,rowNum,0);
        buttonArea->addWidget(tmpInput,rowNum,1);
        rowNum++;
    }

    vLayout->addLayout(buttonArea);
}

void DebugAgaveAppPanel::commandInvoked()
{
    if (waitingOnCommand)
    {
        return;
    }

    qDebug("Agave Command Test Invoked");
    QString selectedApp = agaveAppList.itemFromIndex(agaveOptionList->currentIndex())->text();
    qDebug("Selected App: %s", qPrintable(selectedApp));

    QString workingDir = myTreeReader->getCurrentSelectedFile().getFullPath();
    qDebug("Working Dir: %s", qPrintable(workingDir));

    QStringList inputList = inputLists.value(selectedApp);
    QMultiMap<QString, QString> allInputs;

    qDebug("Input List:");
    for (auto itr = inputList.cbegin(); itr != inputList.cend(); itr++)
    {
        QLineEdit * theInput = this->getOwnedWidget()->findChild<QLineEdit *>(*itr);
        if (theInput != NULL)
        {
            allInputs.insert((*itr),theInput->text());
            qDebug("%s : %s", qPrintable(*itr), qPrintable(theInput->text()));
        }
    }

    RemoteDataReply * theTask = dataConnection->runRemoteJob(selectedApp,allInputs,workingDir);
    if (theTask == NULL)
    {
        qDebug("Unable to invoke task");
        //TODO: give reasonable error
        return;
    }
    waitingOnCommand = true;
    expectedCommand = selectedApp;
    QObject::connect(theTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(commandReply(RequestState,QJsonDocument*)));
}

void DebugAgaveAppPanel::commandReply(RequestState finalState, QJsonDocument *)
{
    waitingOnCommand = false;
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}

void SimpleNameValPanel::setupOwnFrame()
{
    QVBoxLayout * vLayout = new QVBoxLayout;

    for (auto itr = indirectParamList.cbegin(); itr != indirectParamList.cend(); itr++)
    {
        QString itemName = (*itr);
        QHBoxLayout * nameValPair = new QHBoxLayout;

        QLabel * headLabel = new QLabel(itemName);
        nameValPair->addWidget(headLabel);

        QLineEdit * dataEntry = new QLineEdit();
        dataEntry->setObjectName(itemName);
        indirectParamBoxes.append(dataEntry);
        nameValPair->addWidget(dataEntry);
        vLayout->addLayout(nameValPair);
    }

    for (auto itr = directParamList.cbegin(); itr != directParamList.cend(); itr++)
    {
        QString itemName = (*itr);
        QHBoxLayout * nameValPair = new QHBoxLayout;

        QLabel * headLabel = new QLabel(itemName);
        nameValPair->addWidget(headLabel);

        QLineEdit * dataEntry = new QLineEdit();
        dataEntry->setObjectName(itemName);
        directParamBoxes.append(dataEntry);
        nameValPair->addWidget(dataEntry);
        vLayout->addLayout(nameValPair);
    }

    startButton = new QPushButton("Begin App");
    QObject::connect(startButton,SIGNAL(clicked(bool)),this,SLOT(appInvoked()));
    vLayout->addWidget(startButton);
    startButton->setVisible(false);

    getOwnedWidget()->setLayout(vLayout);
}

void SimpleNameValPanel::frameNowVisible()
{
    QObject::connect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
    myTreeReader->resendSelectedFile();
}

void SimpleNameValPanel::frameNowInvisible()
{
    QObject::disconnect(myTreeReader, SIGNAL(newFileSelected(FileMetaData *)), this, SLOT(selectedFileChanged(FileMetaData *)));
}

void SimpleNameValPanel::selectedFileChanged(FileMetaData * newSelection)
{
    if (newSelection->getFileType() == FileType::DIR)
    {
        startButton->setVisible(true);
    }
}

void SimpleNameValPanel::appInvoked()
{
    qDebug("Beginning CFD task");
    FileMetaData fileData = myTreeReader->getCurrentSelectedFile();

    QMultiMap<QString, QString> inputList;
    QString fullIndirectInput;
    for (auto itr = indirectParamBoxes.cbegin(); itr != indirectParamBoxes.cend(); itr++)
    {
        if (!((*itr)->text().isEmpty()))
        {
            fullIndirectInput.append((*itr)->objectName());
            fullIndirectInput.append(" ");
            fullIndirectInput.append((*itr)->text());
            fullIndirectInput.append(" ");
        }
    }
    inputList.insert(composedParamName, fullIndirectInput);

    for (auto itr = directParamBoxes.cbegin(); itr != directParamBoxes.cend(); itr++)
    {
        inputList.insert((*itr)->objectName(), (*itr)->text());
    }

    RemoteDataReply * remoteTask = dataConnection->runRemoteJob(appName,inputList,fileData.getFullPath());
    if (remoteTask == NULL)
    {
        //TODO: give reasonable error
        return;
    }
    QObject::connect(remoteTask, SIGNAL(haveJobReply(RequestState,QJsonDocument*)),
                     this, SLOT(finishedAppInvoke(RequestState,QJsonDocument*)));
}

void SimpleNameValPanel::finishedAppInvoke(RequestState finalState, QJsonDocument *)
{
    if (finalState != RequestState::GOOD)
    {
        //TODO: give reasonable error
        return;
    }

    //TODO: ask for refresh of relevant containing folder
}
