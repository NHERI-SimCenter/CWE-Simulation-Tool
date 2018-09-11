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

#include "duplicate_case_popup.h"
#include "ui_duplicate_case_popup.h"

#include "remoteFiles/filetreenode.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "mainWindow/cwe_mainwindow.h"

#include "cwe_interfacedriver.h"

#include "cwe_globals.h"

Duplicate_Case_Popup::Duplicate_Case_Popup(FileNodeRef toClone, CWE_MainWindow *controlWindow, QWidget *parent) :
    CWE_Popup(controlWindow, parent),
    ui(new Ui::Duplicate_Case_Popup)
{
    ui->setupUi(this);

    if (!cwe_globals::get_CWE_Driver()->inOfflineMode())
    {
        ui->primary_remoteFileTree->setModelLink(cwe_globals::get_file_handle());
    }

    clonedFolder = toClone;
}

Duplicate_Case_Popup::~Duplicate_Case_Popup()
{
    delete ui;
}

void Duplicate_Case_Popup::button_create_copy_clicked()
{
    if (cwe_globals::get_CWE_Driver()->inOfflineMode())
    {
        this->close();
        return;
    }

    /* take emergency exit if nothing has been selected */
    FileNodeRef selectedFile = ui->primary_remoteFileTree->getSelectedFile();
    if (selectedFile.isNil())
    {
        cwe_globals::displayPopup("Please select a folder to place the new case.");
        return;
    }
    if (selectedFile.getFileType() != FileType::DIR)
    {
        cwe_globals::displayPopup("Please select a folder to place the new case.");
        return;
    }

    /* OK, something has been selected */
    CFDcaseInstance * newCase;

    QString newCaseName = ui->lineEdit_newCaseName->text();

    if (!cwe_globals::isValidFolderName(newCaseName))
    {
        cwe_globals::displayPopup("Please input a valid folder name. Folder names should not include any special characters. (!, @, #, $, %, ^, etc.)");
        return;
    }

    /* we are cloning from an existing case */
    if (clonedFolder.isNil())
    {
        cwe_globals::displayPopup("Folder to duplicated is invalid.", "ERROR");
        return;
    }

    CFDcaseInstance * tempCase = myMainWindow->getCaseFromFolder(clonedFolder);
    if (tempCase == nullptr)
    {
        cwe_globals::displayPopup("Folder to duplicated is invalid.", "ERROR");
        return;
    }

    if (tempCase != myMainWindow->getCurrentCase())
    {
        tempCase->deleteLater();
    }

    CaseState dupState = tempCase->getCaseState();

    if (dupState == CaseState::INVALID)
    {
        cwe_globals::displayPopup("ERROR: Can only duplicate CFD cases managed by CWE. Please select a valid folder containing a case for duplication.");
        return;
    }
    if (dupState == CaseState::LOADING)
    {
        cwe_globals::displayPopup("Please wait for case folder to load before attempting to duplicate.");
        return;
    }
    if (dupState != CaseState::READY)
    {
        cwe_globals::displayPopup("Unable to duplicate case. Please check that the case does not have an active job.");
        return;
    }
    newCase = new CFDcaseInstance();
    if (!newCase->duplicateCase(newCaseName, selectedFile, clonedFolder))
    {
        cwe_globals::displayPopup("Unable to contact design safe. Please wait and try again.", "Network Issue");
        newCase->deleteLater();
        return;
    }

    if (newCase->getCaseState() != CaseState::OP_INVOKE)
    {
        cwe_globals::displayPopup("Cannot create new case due to internal error.");
        newCase->deleteLater();
        return;
    }

    //Set new case will signal the other panels so that they can get configurations
    myMainWindow->setCurrentCase(newCase);

    /* time to switch to the ParameterTab */
    myMainWindow->switchToParameterTab();
    this->close();
}
