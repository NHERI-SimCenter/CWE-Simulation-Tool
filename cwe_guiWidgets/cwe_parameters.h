/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#ifndef CWE_PARAMETERS_H
#define CWE_PARAMETERS_H

#include "cwe_super.h"

#include <QVector>
#include <QLabel>
#include <QJsonObject>

#include "SimCenter_widgets/sctrstates.h"

class CWE_MainWindow;

class CWE_StageStatusTab;
class CWE_GroupTab;
class CWE_ParamTab;
class SCtrMasterDataWidget;

struct VARIABLE_TYPE;

enum class CaseState;
enum class StageState;

namespace Ui {
class CWE_Parameters;
}

class CWE_Parameters : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Parameters(QWidget *parent = 0);
    ~CWE_Parameters();

    virtual void linkMainWindow(CWE_MainWindow *theMainWin);

    void setSaveAllButtonDisabled(bool newSetting);
    void setSaveAllButtonEnabled(bool newSetting);

private slots:
    void save_all_button_clicked();
    void run_button_clicked();
    void cancel_button_clicked();
    void results_button_clicked();
    void rollback_button_clicked();

    void newCaseGiven();
    void newCaseState(CaseState newState);

    void stageSelected(CWE_ParamTab * chosenTab);
    void groupSelected(CWE_ParamTab * chosenTab);

private:
    bool checkButtonEnactReady();
    bool paramsChanged();

    void setButtonsAccordingToStage();
    void setVisibleAccordingToStage();

    void setHeaderLabels();

    void createStageTabs();
    void createGroupTabs();
    void createParamWidgets();
    void addVariable(QString varName, VARIABLE_TYPE &theVariable);

    void clearStageTabs();
    void clearGroupTabs();
    void clearParamScreen();

    static QString getStateText(StageState theState);

    Ui::CWE_Parameters *ui;

    QVector<CWE_StageStatusTab *> stageTabList;
    QVector<CWE_GroupTab *> groupTabList;
    QVector<SCtrMasterDataWidget *> paramWidgetList;

    CWE_StageStatusTab * selectedStage = NULL;
    CWE_GroupTab * selectedGroup = NULL;

    QLabel * loadingLabel = NULL;
};

#endif // CWE_PARAMETERS_H
