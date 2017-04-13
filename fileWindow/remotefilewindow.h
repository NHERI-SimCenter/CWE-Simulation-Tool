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

#ifndef REMOTEFILEWINDOW_H
#define REMOTEFILEWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTreeView>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QLabel>
#include <QPoint>
#include <QMenu>
#include <QCursor>

//NOTE: FILENAME MUST == 0 for these functions to work.
//The other columns can be changed
enum class FileColumn : int {FILENAME = 0,
                             TYPE = 1,
                             SIZE = 2,
                             LAST_CHANGED = 3,
                             FORMAT = 4,
                             MIME_TYPE = 5,
                             PERMISSIONS = 6};

class RemoteDataInterface;
class FileMetaData;
class FileTreeNode;
class VWTinterfaceDriver;
class FileOperator;
enum class RequestState;

namespace Ui {
class RemoteFileWindow;
}

class RemoteFileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RemoteFileWindow(VWTinterfaceDriver * newDataLink, QWidget *parent = 0);
    ~RemoteFileWindow();

    void resendSelectedFile();
    FileMetaData getCurrentSelectedFile();
    void resetFileData();

    void updateFileInfo(QList<FileMetaData> * fileDataList);

    void lsClosestNode(QString fullPath);
    void lsClosestNodeToParent(QString fullPath);

signals:
    void newFileSelected(FileMetaData * newFileData);

private slots:
    void folderExpanded(QModelIndex itemOpened);
    void fileEntryTouched(QModelIndex fileIndex);
    void needRightClickMenu(QPoint pos);

private:
    Ui::RemoteFileWindow *ui;

    QString getFilePathForNode(QModelIndex dataIndex);
    FileTreeNode * getFileNodeFromPath(QString filePath);
    FileTreeNode * getDirNearestFromPath(QString filePath);

    //Note: if not found, will return NULL and call translateFileDataToModel(), to resync
    //If input is NULL, return NULL, but don't resync
    FileTreeNode * getNodeFromModel(QStandardItem * toFind);
    QStandardItem * getModelEntryFromNode(FileTreeNode * toFind);

    void totalResetErrorProcedure();
    void translateFileDataToModel();
    void translateFileDataRecurseHelper(FileTreeNode * currentFile, QStandardItem * currentModelEntry);

    bool fileInModel(FileTreeNode * toFind, QStandardItem * compareTo);
    void changeModelFromFile(QStandardItem * targetRow, FileTreeNode * dataSource);
    void newModelRowFromFile(QStandardItem * parentItem, FileTreeNode * dataSource);

    bool columnInUse(int i);
    QString getRawColumnData(int i, FileMetaData * rawFileData);

    FileOperator * myFileOperator;

    const int tableNumCols = 7;
    const QStringList shownHeaderLabelList = {"File Name","Type","Size","Last Changed",
                                   "Format","mimeType","Permissions"};
    const QStringList hiddenHeaderLabelList = {"name","type","length","lastModified",
                                   "format","mimeType","permissions"};

    QTreeView * linkedFileView = NULL;
    QLabel * selectedFileDisplay = NULL;

    FileTreeNode * rootFileNode = NULL;
    QStandardItemModel dataStore;
    FileTreeNode * selectedItem = NULL;
};

#endif // REMOTEFILEWINDOW_H
