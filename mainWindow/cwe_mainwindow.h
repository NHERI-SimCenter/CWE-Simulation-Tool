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

#ifndef CWE_MAINWINDOW_H
#define CWE_MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QStackedLayout>
#include <QPushButton>

#include "cwe_guiWidgets/cwe_defines.h"
#include "cwe_guiWidgets/cwe_landing.h"
#include "cwe_guiWidgets/cwe_create_simulation_old.h"
#include "cwe_guiWidgets/cwe_file_manager.h"
#include "cwe_guiWidgets/cwe_manage_simulation.h"
#include "cwe_guiWidgets/cwe_simulation_details.h"
#include "cwe_guiWidgets/cwe_task_list.h"
#include "cwe_guiWidgets/cwe_help.h"

#include "../AgaveExplorer/utilFuncs/copyrightdialog.h"
#include "vwtinterfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"

namespace Ui {
class CWE_MainWindow;
}

class CFDcaseInstance;
class cwe_state_label;

class CWE_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CWE_MainWindow(VWTinterfaceDriver *newDriver, QWidget *parent = 0);
    ~CWE_MainWindow();

    void runOfflineSetupSteps();
    void runSetupSteps();

    void attachCaseSignals(CFDcaseInstance * newCase);

private slots:
    void tabChanged(int);
    void newActiveCase();
    void switchToParameterTab();
    void switchToResultsTab();
    void switchToCreateTab();

    void menuExit();
    void menuCopyInfo();

    void on_action_Quit_triggered();

    void on_actionCreate_New_Simulation_triggered();
    void on_actionManage_Simulation_triggered();
    void on_actionOpen_triggered();
    void on_actionOpen_existing_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionAbout_CWE_triggered();
    void on_actionHelp_triggered();
    void on_action_Landing_Page_triggered();
    void on_actionManage_Remote_Jobs_triggered();
    void on_actionTutorials_and_Help_triggered();

    void on_actionManage_and_Download_Files_triggered();

private:
    Ui::CWE_MainWindow *ui;
    int switchToTab(int idx);

    VWTinterfaceDriver     *myDriver;
    RemoteDataInterface    *dataLink;
    cwe_state_label        *stateLabel = NULL;
};

#endif // CWE_MAINWINDOW_H
