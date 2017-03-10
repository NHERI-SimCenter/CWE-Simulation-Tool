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

#include "filetreemodelreader.h"

#include "../vwtinterfacedriver.h"
#include "agavehandler.h"
#include "agavetaskreply.h"

#include "../programWindows/deleteconfirm.h"
#include "../programWindows/singlelinedialog.h"
#include "../programWindows/errorpopup.h"

//TODO: This class is a total mess from beginning to end. PRS
//TODO: Many inefficient nested function calls. Mostly because I was writing it blind to how it should work. PRS
//TODO: Next task will be rewrite of this and corresponging .h interface. PRS

FileTreeModelReader::FileTreeModelReader(AgaveHandler * newAgaveLink, QTreeView * newLinkedFileView, QLabel * fileHeaderLabel)
{
    linkedFileView = newLinkedFileView;
    agaveLink = newAgaveLink;
    filesLabel = fileHeaderLabel;
    QObject::connect(agaveLink, SIGNAL(sendFileData(QJsonValue)), this, SLOT(refreshFileInfo(QJsonValue)));
    QObject::connect(linkedFileView, SIGNAL(expanded(QModelIndex)), this, SLOT(folderExpanded(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(clicked(QModelIndex)), this, SLOT(fileEntryTouched(QModelIndex)));
    QObject::connect(linkedFileView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(needRightClickMenu(QPoint)));

    linkedFileView->setModel(&dataStore);

    selectedItem = dataStore.indexFromItem(dataStore.invisibleRootItem());
}

QString FileTreeModelReader::getFilePathForNode(QModelIndex dataIndex)
{
    QString searchString;
    QStandardItem * rootNode = dataStore.invisibleRootItem();
    QStandardItem * openedItem = dataStore.itemFromIndex(dataIndex);
    //TODO: Docs not clear what happens if QModelIndex is invalid

    QStandardItem * scannedItem = openedItem;

    while ((scannedItem != NULL) && (scannedItem != rootNode))
    {
        searchString.prepend(scannedItem->text());
        searchString.prepend("/");
        scannedItem = scannedItem->parent();
    }

    return searchString;
}

QMap<QString, QString> FileTreeModelReader::getDataForEntry(QModelIndex dataIndex)
{
    QMap<QString, QString> ret;

    QStandardItem * openedItem = dataStore.itemFromIndex(dataIndex);

    if (openedItem == NULL)
    {
        return ret;
    }

    QStandardItem * parentItem = openedItem->parent();
    int rowIndex = openedItem->row();

    if (parentItem == NULL)
    {
        parentItem = dataStore.invisibleRootItem();
    }

    for (int i = 0; i < tableNumCols; i++)
    {
        ret.insert(hiddenHeaderLabelList.at(i), parentItem->child(rowIndex,i)->text());
    }
    return ret;
}

QModelIndex FileTreeModelReader::getCurrentSelectedFile()
{
    return selectedItem;
}

void FileTreeModelReader::setTreeViewVisibility(bool isVisible)
{
    linkedFileView->setVisible(isVisible);
    filesLabel->setVisible(isVisible);
}

void FileTreeModelReader::setRightClickMenuEnabled(bool newSetting)
{
    rightClickEnabled = newSetting;
}

void FileTreeModelReader::resetFileData()
{
    dataStore.clear();
    dataStore.setColumnCount(tableNumCols);
    dataStore.setHorizontalHeaderLabels(shownHeaderLabelList);
    linkedFileView->hideColumn((int)FileColumn::MIME_TYPE);
    linkedFileView->hideColumn((int)FileColumn::PERMISSIONS);
    linkedFileView->hideColumn((int)FileColumn::FORMAT);
    linkedFileView->hideColumn((int)FileColumn::LAST_CHANGED);
    //TODO: Adjust column size defaults;

    QStandardItem * rootNode = dataStore.invisibleRootItem();
    rootNode->appendRow(this->getLoadingPlaceholderRow());
    agaveLink->retriveDirListing("/");
}

void FileTreeModelReader::refreshFolderFromPath(QString pathToRefresh)
{
    //We traverse from root to the file specified, stoping when we either
    //Do not know of the relevant file, or a reach a non folder
    //And then refresh the deepest known folder

    QStringList filePaths = pathToRefresh.split('/');
    if (filePaths.length() == 0) return;

    QStandardItem * searchItem = dataStore.invisibleRootItem();
    for (auto itr = filePaths.cbegin(); itr != filePaths.cend(); itr++)
    {
        if (itr->isEmpty()) continue;
        if (searchItem->rowCount() == 0)
        {
            refreshFolderFromNode(searchItem);
            return;
        }
        bool foundMatch = false;
        for (int i = 0; i < searchItem->rowCount(); i++)
        {
            QStandardItem * candidateItem = searchItem->child(i,(int)FileColumn::FILENAME);
            QStandardItem * candidateType = searchItem->child(i,(int)FileColumn::TYPE);
            if (candidateItem->text() == (*itr))
            {
                if (candidateType->text() != "dir")
                {
                    refreshFolderFromNode(searchItem);
                    return;
                }
                foundMatch = true;
                searchItem = candidateItem;
                i = searchItem->rowCount();
            }
        }
        if (!foundMatch)
        {
            refreshFolderFromNode(searchItem);
            return;
        }
    }
    refreshFolderFromNode(searchItem);
}

void FileTreeModelReader::refreshFolderFromNode(QStandardItem * modelNode)
{
    if (modelNode != dataStore.invisibleRootItem())
    {
        agaveLink->retriveDirListing(getFilePathForNode(modelNode->index()));
    }
    else
    {
        agaveLink->retriveDirListing("/");
    }
}

void FileTreeModelReader::refreshFileInfo(QJsonArray fileList)
{
    if (!insertFileData(fileList.toArray()))
    {
        //if we have a data mismatch we reset everything
        //TODO: this should be more graceful
        resetFileData();
    }
}

void FileTreeModelReader::folderExpanded(QModelIndex itemOpened)
{
    if (!needDataRefresh(itemOpened))
    {
        return;
    }
    qDebug("File Path: %s", getFilePathForNode(itemOpened).toStdString().c_str());
    agaveLink->retriveDirListing(getFilePathForNode(itemOpened));
}

void FileTreeModelReader::fileEntryTouched(QModelIndex fileIndex)
{
    QStandardItem * touchedItem = dataStore.itemFromIndex(fileIndex);

    if (touchedItem == NULL)
    {
        return;
    }

    int rowNum = touchedItem->row();
    int colNum = touchedItem->column();
    if (colNum != 0)
    {
        QStandardItem * parentItem = touchedItem->parent();
        if (parentItem == NULL)
        {
            parentItem = dataStore.invisibleRootItem();
        }
        selectedItem = parentItem->child(rowNum,0)->index();
    }
    else
    {
        selectedItem = touchedItem->index();
    }

    emit newFileSelected(selectedItem);
    return;
}

void FileTreeModelReader::needRightClickMenu(QPoint pos)
{
    QModelIndex targetIndex = linkedFileView->indexAt(pos);
    QStandardItem * touchedItem = dataStore.itemFromIndex(targetIndex);

    //If we did not click anything, we should return
    if (touchedItem == NULL) return;

    fileEntryTouched(targetIndex);
    QMap<QString,QString> rawFileData = getDataForEntry(getCurrentSelectedFile());

    if (rawFileData.value("format") == "Fetching Data") return;
    if (rawFileData.value("type") == "Empty Folder") return;

    QMenu fileMenu;

    if (fileOperationPending)
    {
        fileMenu.addAction("File Operation In Progress . . .");
        fileMenu.exec(QCursor::pos());
        return;
    }

    fileMenu.addAction("Copy To . . .",SLOT(sendCopyReq()));
    fileMenu.addAction("Move To . . .",SLOT(sendMoveReq()));
    fileMenu.addAction("Rename",SLOT(sendRenameReq()));
    //We don't let the user delete the username folder
    if (dataStore.itemFromIndex(targetIndex)->parent() != NULL)
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Delete",SLOT(sendDeleteReq()));
        fileMenu.addSeparator();
    }
    if (rawFileData.value("type") == "dir")
    {
        fileMenu.addAction("Upload File Here",SLOT(sendUploadReq()));
        fileMenu.addAction("Create New Folder",SLOT(sendCreateFolderReq()));
    }
    else
    {
        fileMenu.addAction("Download File",SLOT(sendDownloadReq()));
    }

    fileMenu.exec(QCursor::pos());
}

void FileTreeModelReader::sendCopyReq()
{
    //TODO
}

void FileTreeModelReader::getCopyResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    //TODO
}

void FileTreeModelReader::sendMoveReq()
{
    //TODO
}

void FileTreeModelReader::getMoveResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    //TODO
}

void FileTreeModelReader::sendRenameReq()
{
    QString reqPath = getFilePathForNode(getCurrentSelectedFile());
    SingleLineDialog newNamePopup("Please type a new file name:", "newname");
    //TODO: NEED lots of verification here
    //First, that file can be renamed
    //Second, that new file name is valid
    if (newNamePopup->exec() != QDialog::Accepted)
    {
        return;
    }

    QStringList URLparams = {reqPath};
    QStringList POSTparams = {newNamePopup->getInputText()};

    AgaveTaskReply * replyHandle = agaveLink->performAgaveQuery("renameFile",&URLparams,&POSTparams);
    if (replyHandle == NULL)
    {
        return;
    }
    QObject::connect(replyHandle, SIGNAL(sendAgaveResultData(QString,RequestState,QMap<QString,QJsonValue>)),
                        this, SLOT(getRenameResult(QString,RequestState,QMap<QString,QJsonValue>)));
    fileOperationPending = true;
}

void FileTreeModelReader::getRenameResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    fileOperationPending = false;
    //TODO: data refresh
}

void FileTreeModelReader::sendDeleteReq()
{
    QString reqPath = getFilePathForNode(getCurrentSelectedFile());
    //TODO: verify file valid for delete
    DeleteConfirm deletePopup(reqPath);
    if (deletePopup->exec() != QDialog::Accepted)
    {
        return;
    }

    QStringList URLparams = {reqPath};

    AgaveTaskReply * replyHandle = agaveLink->performAgaveQuery("fileDelete",&URLparams);
    if (replyHandle == NULL)
    {
        return;
    }
    QObject::connect(replyHandle, SIGNAL(sendAgaveResultData(QString,RequestState,QMap<QString,QJsonValue>)),
                        this, SLOT(getDeleteResult(QString,RequestState,QMap<QString,QJsonValue>)));
    fileOperationPending = true;
}

void FileTreeModelReader::getDeleteResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    fileOperationPending = false;
    //TODO: data refresh
}

void FileTreeModelReader::sendUploadReq()
{
    SingleLineDialog uploadNamePopup("Please input full path of file to upload:", "");
    QString reqPath = getFilePathForNode(getCurrentSelectedFile());
    //TODO: NEED lots of verification here
    //First, valid folder to upload to
    //Second, that uploaded file exists and is valid
    if (uploadNamePopup->exec() != QDialog::Accepted)
    {
        return;
    }

    QStringList URLparams = {reqPath};
    QStringList POSTparams = {uploadNamePopup->getInputText()};

    AgaveTaskReply * replyHandle = agaveLink->performAgaveQuery("fileUpload",&URLparams,&POSTparams);
    if (replyHandle == NULL)
    {
        return;
    }
    QObject::connect(replyHandle, SIGNAL(sendAgaveResultData(QString,RequestState,QMap<QString,QJsonValue>)),
                        this, SLOT(getUploadResult(QString,RequestState,QMap<QString,QJsonValue>)));
    fileOperationPending = true;
}

void FileTreeModelReader::getUploadResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    fileOperationPending = false;
    //TODO: data refresh
}

void FileTreeModelReader::sendCreateFolderReq()
{
    SingleLineDialog newFolderNamePopup("Please input a name for the new folder:", "newFolder1");
    QString reqPath = getFilePathForNode(getCurrentSelectedFile());
    //TODO: verification here
    //First, valid folder to create in
    //Second, that new name is valid
    if (newFolderNamePopup->exec() != QDialog::Accepted)
    {
        return;
    }

    QStringList URLparams = {reqPath};
    QStringList POSTparams = {uploadNamePopup->getInputText()};

    AgaveTaskReply * replyHandle = agaveLink->performAgaveQuery("newFolder",&URLparams,&POSTparams);
    if (replyHandle == NULL)
    {
        return;
    }
    QObject::connect(replyHandle, SIGNAL(sendAgaveResultData(QString,RequestState,QMap<QString,QJsonValue>)),
                        this, SLOT(getCreateFolderResult(QString,RequestState,QMap<QString,QJsonValue>)));
    fileOperationPending = true;
}

void FileTreeModelReader::getCreateFolderResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    fileOperationPending = false;
    //TODO: data refresh
}

void FileTreeModelReader::sendDownloadReq()
{
    //TODO
}

void FileTreeModelReader::getDownloadResult(QString taskID, RequestState resultState, QMap<QString,QJsonValue> rawData)
{
    //TODO
}

bool FileTreeModelReader::insertFileData(QJsonArray fileList)
{
    //TODO: This function is a mess and needs cleaning to be more elegant
    QString searchPath;

    for (auto itr = fileList.constBegin(); itr != fileList.constEnd(); ++itr)
    {
        if (!(*itr).isObject()) return false;

        if (((*itr).toObject().value("name").toString()) == ".")
        {
            searchPath = ((*itr).toObject().value("path").toString());
            break;
        }
    }

    if (searchPath.isEmpty())
    {
        return false;
    }

    QStandardItem * targetNode = dataStore.invisibleRootItem();
    //if this request is for the root folder, then we need to add to the top level
    //else we must find the correct folder already in to the tree and add there

    bool targetingRoot = false;
    if (targetNode->child(0,(int)FileColumn::FORMAT)->text() == "Fetching Data")
    {
        if (searchPath.at(0) == '/')
        {
            searchPath.remove(0,1);
        }
        if (searchPath.contains('/'))
        {
            return false;
        }
        if (searchPath.contains('\\'))
        {
            return false;
        }
        targetingRoot = true;
    }
    else
    {
        if (searchPath.at(0) == '/')
        {
            searchPath.remove(0,1);
        }
        QStringList filePathList = searchPath.split('/');

        int entryCountdown = filePathList.length();
        for (auto itr = filePathList.cbegin(); itr != filePathList.cend(); ++itr )
        {
            for (int i = 0; i < targetNode->rowCount(); i++)
            {
                QStandardItem * oneChild = targetNode->child(i,0);
                if (oneChild->text() == (*itr))
                {
                    targetNode = oneChild;
                    i = targetNode->rowCount();
                    entryCountdown--;
                }
            }
        }
        if (entryCountdown != 0)
        {
            return false;
        }
    }

    if (targetingRoot == true)
    {
        //If we are looking at root, we need to add the true root entry and redirect to that
        targetNode = dataStore.invisibleRootItem()->child(0);
    }

    //Somehow, we clear out everything below the target node
    targetNode->removeRows(0,targetNode->rowCount());

    //Then we one-by-one, insert each file into to that location
    int currRow = 0;
    bool haveFiles = false;
    for (auto itr = fileList.constBegin(); itr != fileList.constEnd(); ++itr)
    {
        if (((*itr).toObject().value("name").toString()) == ".")
        {
            if (targetingRoot)
            {
                for (int i = 0; i < tableNumCols; i++)
                {
                    if ((FileColumn)i == FileColumn::FILENAME)
                    {
                        QString rawPathName = (*itr).toObject().value("path").toString();
                        rawPathName.remove(0,1);
                        dataStore.invisibleRootItem()->child(0,i)->setText(rawPathName);
                    }
                    else if ((FileColumn)i == FileColumn::SIZE)
                    {
                        dataStore.invisibleRootItem()->child(0,i)->setText("");
                    }
                    else
                    {
                        dataStore.invisibleRootItem()->child(0,i)->setText((*itr).toObject().value(hiddenHeaderLabelList.at(i)).toString());
                    }
                }
            }
            continue;
        }

        QList<QStandardItem*> newEntry;
        for (int i = 0; i < tableNumCols; i++)
        {
            if ((FileColumn)i == FileColumn::SIZE)
            {
                if ((*itr).toObject().value("type").toString() == "dir")
                {
                    newEntry.append(new QStandardItem(""));
                }
                else
                {
                    int sizeVal = (*itr).toObject().value(hiddenHeaderLabelList.at(i)).toInt();
                    QString sizeString = QString::number(sizeVal);
                    //TODO: Change this to be more human readable?
                    //Right way: new col stores size in bytes,
                    //visible col shows in k,m,g . . .
                    newEntry.append(new QStandardItem(sizeString));
                }
            }
            else
            {
                newEntry.append(new QStandardItem((*itr).toObject().value(hiddenHeaderLabelList.at(i)).toString()));
            }
        }
        targetNode->appendRow(newEntry);
        haveFiles = true;

        if ((*itr).toObject().value("type").toString() == "dir")
        {
            QStandardItem * folderItem = targetNode->child(currRow);
            folderItem->appendRow(getLoadingPlaceholderRow());
        }
        currRow++;
    }

    if (haveFiles == false)
    {
        QList<QStandardItem*> newEntry;

        for (int i = 0; i < tableNumCols; i++)
        {
            if ((FileColumn)i == FileColumn::TYPE)
            {
                newEntry.append(new QStandardItem("Empty Folder"));
            }
            else
            {
                newEntry.append(new QStandardItem(""));
            }
        }
        targetNode->appendRow(newEntry);
    }

    //We re-select the current item
    fileEntryTouched(targetNode->index());

    return true;
}

QList<QStandardItem*> FileTreeModelReader::getLoadingPlaceholderRow()
{
    QList<QStandardItem*> ret;

    for (int i = 0; i < tableNumCols; i++)
    {
        if ((FileColumn)i == FileColumn::FILENAME)
        {
            ret.append(new QStandardItem("Loading . . ."));
        }
        else if ((FileColumn)i == FileColumn::FORMAT)
        {
            ret.append(new QStandardItem("Fetching Data"));
        }
        else
        {
            ret.append(new QStandardItem(""));
        }
    }
    return ret;
}

bool FileTreeModelReader::needDataRefresh(QModelIndex dataIndex)
{
    QStandardItem * openedItem = dataStore.itemFromIndex(dataIndex);
    if (!openedItem->hasChildren())
    {
        return false;
    }
    QStandardItem * childEntry = openedItem->child(0,(int)FileColumn::FORMAT);
    QString compareString = childEntry->text();
    if (compareString == "Fetching Data")
    {
        return true;
    }
    return false;
}
