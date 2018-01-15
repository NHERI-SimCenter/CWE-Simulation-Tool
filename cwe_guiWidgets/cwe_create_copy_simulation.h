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

#ifndef CWE_CREATE_COPY_SIMULATION_H
#define CWE_CREATE_COPY_SIMULATION_H

#include "cwe_super.h"

#include <QPushButton>
#include <QRadioButton>
#include <QJsonObject>

class CFDanalysisType;

struct CASE_TYPE_DATA {
    QRadioButton         *radioBtn;
    QPushButton          *pbtn;
    CFDanalysisType      *templateData;
};

namespace Ui {
class CWE_Create_Copy_Simulation;
}

class CWE_Create_Copy_Simulation : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Create_Copy_Simulation(QWidget *parent = 0);
    ~CWE_Create_Copy_Simulation();

    virtual void linkDriver(VWTinterfaceDriver * theDriver);

private slots:
    void on_pBtn_create_copy_clicked();
    void on_tabWidget_currentChanged(int index);
    void selectCaseTemplate();

private:
    Ui::CWE_Create_Copy_Simulation *ui;
    void populateCaseTypes();
    void create_new_case_from_template(QString filename);

    CFDanalysisType * selectedTemplate = NULL;

    QVector<CASE_TYPE_DATA> caseTypeDataList;
};

#endif // CWE_CREATE_COPY_SIMULATION_H
