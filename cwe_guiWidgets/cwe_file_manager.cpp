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

#include "cwe_file_manager.h"
#include "ui_cwe_file_manager.h"

#include "../AgaveClientInterface/filemetadata.h"
#include "../AgaveClientInterface/remotedatainterface.h"

#include "remoteFiles/filetreenode.h"
#include "remoteFiles/fileoperator.h"
#include "utilFuncs/singlelinedialog.h"

#include "mainWindow/cwe_mainwindow.h"

#include "cwe_interfacedriver.h"
#include "cwe_globals.h"

CWE_file_manager::CWE_file_manager(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_file_manager)
{
    ui->setupUi(this);

    // Creates our new model and populate
    localFileModel = new QFileSystemModel(this);

    // Set filter
    localFileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    ui->localTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Attach the model to the view
    ui->localTreeView->setModel(localFileModel);
    ui->localTreeView->header()->resizeSection(0,200);

    // QFileSystemModel requires root path
    localFileModel->setRootPath(QDir::homePath());
    ui->localTreeView->setRootIndex(localFileModel->setRootPath(QDir::homePath()));
    setControlsEnabled(false);
}

CWE_file_manager::~CWE_file_manager()
{
    delete ui;
}

void CWE_file_manager::linkMainWindow(CWE_MainWindow *theMainWin)
{
    CWE_Super::linkMainWindow(theMainWin);
    if (!cwe_globals::get_CWE_Driver()->inOfflineMode())
    {
        ui->remoteTreeView->setModelLink(theMainWindow->getFileModel());
        QObject::connect(ui->remoteTreeView, SIGNAL(customContextMenuRequested(QPoint)),
                         this, SLOT(customFileMenu(QPoint)));
        QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileOpDone(RequestState,QString)),
                         this, SLOT(remoteOpDone(RequestState,QString)));
        QObject::connect(cwe_globals::get_file_handle(), SIGNAL(fileOpStarted()),
                         this, SLOT(remoteOpStarted()));
        setControlsEnabled(true);
    }
}

void CWE_file_manager::on_pb_upload_clicked()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Currently running file operation. Please Wait.");
        return;
    }

    FileNodeRef targetFile = ui->remoteTreeView->getSelectedFile();

    if ((targetFile.isNil()) || (targetFile.getFileType() != FileType::DIR))
    {
        cwe_globals::displayPopup("Please select a destination folder to upload to.");
        return;
    }

    QModelIndex localSelectIndex = ui->localTreeView->currentIndex();
    QFileInfo fileData = localFileModel->fileInfo(localSelectIndex);

    if (fileData.isDir())
    {
        cwe_globals::get_file_handle()->enactRecursiveUpload(targetFile, fileData.absoluteFilePath());
    }
    else if (fileData.isFile())
    {
        cwe_globals::get_file_handle()->sendUploadReq(targetFile, fileData.absoluteFilePath());
    }
    else
    {
        cwe_globals::displayPopup("Please select one file or folder for upload.");
        return;
    }

    expectingOp = true;

    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Error: Unable to start file operation. Please try again.");
        return;
    }

    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::on_pb_download_clicked()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Currently running file operation. Please Wait.");
        return;
    }

    FileNodeRef targetFile = ui->remoteTreeView->getSelectedFile();

    if (targetFile.isNil())
    {
        cwe_globals::displayPopup("Please select a file or folder to download");
        return;
    }

    QModelIndex localSelectIndex = ui->localTreeView->currentIndex();
    QFileInfo fileData = localFileModel->fileInfo(localSelectIndex);

    if (!fileData.isDir())
    {
        cwe_globals::displayPopup("Please select exactly 1 local folder to download to");
        return;
    }

    QString localPath = fileData.absoluteFilePath();

    if (targetFile.getFileType() == FileType::FILE)
    {
        #ifdef Q_OS_WIN
            localPath = localPath.append('\\');
        #else
            localPath = localPath.append('/');
        #endif
        localPath = localPath.append(targetFile.getFileName());

        cwe_globals::get_file_handle()->sendDownloadReq(targetFile, localPath);
    }
    else if (targetFile.getFileType() == FileType::DIR)
    {
        cwe_globals::get_file_handle()->enactRecursiveDownload(targetFile, localPath);
    }

    if (!cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Error: Unable to start file operation. Please check that the local file does not already exist and try again.");
        return;
    }

    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::copyMenuItem()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Currently running file operation. Please Wait.");
        return;
    }

    SingleLineDialog newNamePopup("Please type a file name to copy to:", "newname");
    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    cwe_globals::get_file_handle()->sendCopyReq(targetNode, newNamePopup.getInputText());

    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::moveMenuItem()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Currently running file operation. Please Wait.");
        return;
    }
    SingleLineDialog newNamePopup("Please type a file name to move to:", "newname");

    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    cwe_globals::get_file_handle()->sendMoveReq(targetNode,newNamePopup.getInputText());
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::refreshMenuItem()
{
    targetNode.enactFolderRefresh();
}

void CWE_file_manager::downloadBufferItem()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("Currently running file operation. Please Wait.");
        return;
    }
    cwe_globals::get_file_handle()->sendDownloadBuffReq(targetNode);
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::remoteOpStarted()
{
    setControlsEnabled(false);
}

void CWE_file_manager::remoteOpDone(RequestState operationStatus, QString message)
{
    setControlsEnabled(true);

    if (!expectingOp) return;
    expectingOp = false;

    if (operationStatus != RequestState::GOOD)
    {
        cwe_globals::displayPopup(message,"File Transfer Error");
    }
    else
    {
        cwe_globals::displayPopup("File Operation Complete","File Manager");
    }
}

void CWE_file_manager::customFileMenu(const QPoint &pos)
{
    QMenu fileMenu;
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        fileMenu.addAction("File Operation In Progress . . .");
        fileMenu.exec(QCursor::pos());
        return;
    }

    QModelIndex targetIndex = ui->remoteTreeView->indexAt(pos);
    ui->remoteTreeView->fileEntryTouched(targetIndex);

    targetNode = ui->remoteTreeView->getSelectedFile();

    //If we did not click anything, we should return
    if (targetNode.isNil()) return;
    if (targetNode.isRootNode()) return;

    if (targetNode.getFileType() == FileType::INVALID) return;

    fileMenu.addAction("Copy To . . .",this, SLOT(copyMenuItem()));
    fileMenu.addAction("Move To . . .",this, SLOT(moveMenuItem()));
    //We don't let the user delete the username folder

    if (targetNode.getFileType() == FileType::FILE)
    {
        fileMenu.addAction("Download Buffer (DEBUG)",this, SLOT(downloadBufferItem()));
    }

    if ((targetNode.getFileType() == FileType::DIR) || (targetNode.getFileType() == FileType::FILE))
    {
        fileMenu.addSeparator();
        fileMenu.addAction("Refresh Data",this, SLOT(refreshMenuItem()));
        fileMenu.addSeparator();
    }

    fileMenu.exec(QCursor::pos());
}

void CWE_file_manager::button_newFolder_clicked()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("File Operation in progress. Please Wait.");
        return;
    }

    targetNode = ui->remoteTreeView->getSelectedFile();

    if (targetNode.getFileType() != FileType::DIR)
    {
        cwe_globals::displayPopup("Please select a folder in which to place the new folder.");
        return;
    }

    SingleLineDialog newFolderNamePopup("Please input a name for the new folder:", "newFolder1");

    if (newFolderNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }
    cwe_globals::get_file_handle()->sendCreateFolderReq(targetNode, newFolderNamePopup.getInputText());
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::button_delete_clicked()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("File Operation in progress. Please Wait.");
        return;
    }

    targetNode = ui->remoteTreeView->getSelectedFile();
    if (targetNode.isNil())
    {
        cwe_globals::displayPopup("Please select a file or folder to delete it.", "File Manager");
        return;
    }
    if (targetNode.isRootNode())
    {
        cwe_globals::displayPopup("Error: We cannot rename or delete your main folder.");
        return;
    }

    if (cwe_globals::get_file_handle()->deletePopup(targetNode))
    {
        cwe_globals::get_file_handle()->sendDeleteReq(targetNode);
        if (cwe_globals::get_file_handle()->operationIsPending())
        {
            setControlsEnabled(false);
            expectingOp = true;
        }
    }
}

void CWE_file_manager::button_rename_clicked()
{
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        cwe_globals::displayPopup("File Operation in progress. Please Wait.");
        return;
    }

    targetNode = ui->remoteTreeView->getSelectedFile();

    if (targetNode.isNil())
    {
        cwe_globals::displayPopup("Please select a file or folder to rename it.", "File Manager");
        return;
    }

    if (targetNode.isRootNode())
    {
        cwe_globals::displayPopup("Error: We cannot rename or delete your main folder.");
        return;
    }

    SingleLineDialog newNamePopup("Please type a new file name:", "newname");

    if (newNamePopup.exec() != QDialog::Accepted)
    {
        return;
    }

    cwe_globals::get_file_handle()->sendRenameReq(targetNode, newNamePopup.getInputText());
    if (cwe_globals::get_file_handle()->operationIsPending())
    {
        setControlsEnabled(false);
        expectingOp = true;
    }
}

void CWE_file_manager::setControlsEnabled(bool newSetting)
{
    ui->pb_upload->setEnabled(newSetting);
    ui->pb_download->setEnabled(newSetting);

    ui->localTreeView->setEnabled(newSetting);
    ui->remoteTreeView->setEnabled(newSetting);

    ui->remoteButton_delete->setEnabled(newSetting);
    ui->remoteButton_newFolder->setEnabled(newSetting);
    ui->remoteButton_rename->setEnabled(newSetting);
}
