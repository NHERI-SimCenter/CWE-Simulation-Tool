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

#ifndef FILETREEMODELREADER_H
#define FILETREEMODELREADER_H

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

enum class FileColumn : int {FILENAME = 0,
                             TYPE = 1,
                             SIZE = 2,
                             LAST_CHANGED = 3,
                             FORMAT = 4,
                             MIME_TYPE = 5,
                             PERMISSIONS = 6};

class VWTinterfaceDriver;
class AgaveHandler;
class SingleLineDialog;
enum class RequestState;

//TODO: This class is a total mess from beginning to end. PRS
//TODO: Many inefficient nested function calls. Mostly because I was writing it blind to how it should work. PRS
//TODO: Next task will be rewrite of this and corresponging .h interface. PRS

class FileTreeModelReader : public QObject
{
    Q_OBJECT
public:
    FileTreeModelReader(VWTinterfaceDriver * theDriver, QTreeView * newLinkedFileView, QLabel * fileHeaderLabel);
    QString getFilePathForNode(QModelIndex dataIndex);
    QMap<QString,QString> getDataForEntry(QModelIndex dataIndex);
    QModelIndex getCurrentSelectedFile();
    void setTreeViewVisibility(bool isVisible);
    void resetFileData();
    void refreshFolderFromPath(QString pathToRefresh);
    void setRightClickMenuEnabled(bool newSetting);

signals:
    void newFileSelected(QModelIndex dataIndex);

private slots:
    void refreshFileInfo(QJsonValue fileList);
    void folderExpanded(QModelIndex itemOpened);
    void fileEntryTouched(QModelIndex fileIndex);
    void needRightClickMenu(QPoint pos);
    void getAgaveResult();

private:
    bool insertFileData(QJsonArray fileList);
    QList<QStandardItem*> getLoadingPlaceholderRow();
    bool needDataRefresh(QModelIndex dataIndex);
    void refreshFolderFromNode(QStandardItem * modelNode);

    bool runRemoteFileTask(QString taskName, QDialog * approvalDialog = NULL, SingleLineDialog * inputDialog = NULL);

    const int tableNumCols = 7;
    const QStringList shownHeaderLabelList = {"File Name","Type","Size","Last Changed",
                                   "Format","mimeType","Permissions"};
    const QStringList hiddenHeaderLabelList = {"name","type","length","lastModified",
                                   "format","mimeType","permissions"};

    QStandardItemModel dataStore;
    QTreeView * linkedFileView;
    QModelIndex selectedItem;
    VWTinterfaceDriver * myDriver;
    QLabel * filesLabel;
    bool rightClickEnabled = false;
    bool fileOperationPending = false;
    QString pendingOperationRefresh;

    AgaveHandler * agaveLink;
};

#endif // FILETREEMODELREADER_H
