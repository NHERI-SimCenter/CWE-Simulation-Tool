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

#ifndef CWE_MANAGE_SIMULATION_H
#define CWE_MANAGE_SIMULATION_H

#include "cwe_super.h"

#include <QStandardItemModel>

class FileNodeRef;
class CWE_MainWindow;
enum class CaseState;
enum class StageState;

namespace Ui {
class CWE_manage_simulation;
}

class CWE_manage_simulation : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_manage_simulation(QWidget *parent = nullptr);
    ~CWE_manage_simulation();

    virtual void linkMainWindow(CWE_MainWindow *theMainWin);

private slots:
    void newFileSelected(FileNodeRef newFile);

    void newCaseGiven();
    void newCaseState(CaseState newState);

    void create_new_case_clicked();
    void duplicate_case_clicked();
    void on_pb_viewParameters_clicked();
    void on_pb_viewResults_clicked();

private:
    void clearSelectView();
    void showSelectView();

    QString getStateText(StageState theState);

    Ui::CWE_manage_simulation *ui;

    QStandardItemModel stageListModel;
};

#endif // CWE_MANAGE_SIMULATION_H
