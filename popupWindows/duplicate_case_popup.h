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

#ifndef DUPLICATE_CASE_POPUP_H
#define DUPLICATE_CASE_POPUP_H

#include "cwe_popup.h"

#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QJsonObject>

#include "../AgaveExplorer/remoteFileOps/filenoderef.h"

class CFDanalysisType;

namespace Ui {
class Duplicate_Case_Popup;
}

class Duplicate_Case_Popup : public CWE_Popup
{
    Q_OBJECT

public:
    explicit Duplicate_Case_Popup(FileNodeRef toClone, CWE_MainWindow * controlWindow, QWidget *parent = 0);
    ~Duplicate_Case_Popup();

private slots:
    void button_create_copy_clicked();
    //void on_tabWidget_currentChanged(int index);

private:
    Ui::Duplicate_Case_Popup *ui;
    void create_new_case_from_template(QString filename);

    FileNodeRef clonedFolder;
};

#endif // DUPLICATE_CASE_POPUP_H
