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

#include <QDesktopWidget>
#include <QPushButton>

#include "../AgaveExplorer/remoteModelViews/remotefilemodel.h"

namespace Ui {
class CWE_MainWindow;
}

class cwe_state_label;
class CWE_InterfaceDriver;
class CFDcaseInstance;
class CFDanalysisType;
enum class CaseState;

class CWE_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CWE_MainWindow(QWidget *parent = 0);
    ~CWE_MainWindow();

    void runSetupSteps();

    void setParameterConfig(QJsonDocument &obj);

    void switchToParameterTab();
    void switchToResultsTab();
    void switchToFilesTab();

    RemoteFileModel * getFileModel();

    CFDcaseInstance * getCurrentCase();
    void setCurrentCase();
    void setCurrentCase(CFDcaseInstance * newCase);
    void setCurrentCase(const FileNodeRef &caseNode);
    void setCurrentCase(CFDanalysisType * newCaseType);

    CFDcaseInstance * getCaseFromType(CFDanalysisType *caseType);
    CFDcaseInstance * getCaseFromFolder(const FileNodeRef &caseNode);

signals:
    void haveNewCase();

private slots:
    void newCaseState(CaseState newState);

    void menuExit();
    void menuCopyInfo();

private:
    void deactivateCurrentCase();

    void changeParamsAndResultsEnabled(bool setting);
    void changeTabEnabled(QWidget *theTab, bool newSetting);

    Ui::CWE_MainWindow *ui;

    CFDcaseInstance * currentCase = NULL;

    RemoteFileModel fileModel;
    cwe_state_label        *stateLabel = NULL;
};

#endif // CWE_MAINWINDOW_H
