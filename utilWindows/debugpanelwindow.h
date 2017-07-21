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

#ifndef PANELWINDOW_H
#define PANELWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStackedWidget>
#include <QLineEdit>

#include <QMap>
#include <QStringList>

class RemoteFileTree;
class FileMetaData;
class FileTreeNode;
class FileOperator;
class RemoteDataInterface;
class VWTinterfaceDriver;
enum class RequestState;

namespace Ui {
class DebugPanelWindow;
}

class DebugPanelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebugPanelWindow(RemoteDataInterface *newDataLink, QWidget *parent = 0);
    ~DebugPanelWindow();

    void startAndShow();

private slots:
    void agaveAppSelected(QModelIndex clickedItem);

    void setTestVisual();
    void setMeshVisual();

    void agaveCommandInvoked();
    void finishedAppInvoke(RequestState finalState, QJsonDocument * rawReply);

    void gotNewRawFile(RequestState authReply, QByteArray * fileBuffer);

    void customFileMenu(QPoint pos);

    void copyMenuItem(FileTreeNode * targetNode);
    void moveMenuItem(FileTreeNode * targetNode);
    void renameMenuItem(FileTreeNode * targetNode);
    void deleteMenuItem(FileTreeNode * targetNode);
    void uploadMenuItem(FileTreeNode * targetNode);
    void createFolderMenuItem(FileTreeNode * targetNode);
    void downloadMenuItem(FileTreeNode * targetNode);
    void compressMenuItem(FileTreeNode * targetNode);
    void decompressMenuItem(FileTreeNode * targetNode);
    void refreshMenuItem(FileTreeNode * targetNode);

private:
    void conditionalPurge(QByteArray ** theArray);

    Ui::DebugPanelWindow *ui;

    FileOperator * theFileOperator;
    RemoteDataInterface * dataLink;
    RemoteFileTree * fileTreeData;

    QStandardItemModel taskListModel;
    QString selectedAgaveApp;

    QByteArray * pointData = NULL;
    QByteArray * faceData = NULL;
    QByteArray * ownerData = NULL;

    QMap<QString, QStringList> agaveParamLists;

    bool waitingOnCommand = false;
};

#endif // PANELWINDOW_H
