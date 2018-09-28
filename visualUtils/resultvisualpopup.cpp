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
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "resultvisualpopup.h"
#include "ui_resultvisualpopup.h"

#include "remoteFiles/filetreenode.h"

#include "CFDanalysis/cwecaseinstance.h"
#include "CFDanalysis/cweanalysistype.h"
#include "cwe_globals.h"

ResultVisualPopup::ResultVisualPopup(CWEcaseInstance *theCase, RESULT_ENTRY * resultDesc, QWidget *parent) :
    ResultProcureBase(parent),
    ui(new Ui::ResultVisualPopup)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, true);

    myCase = theCase;
    if (myCase == nullptr)
    {
        cwe_globals::displayFatalPopup("Internal error: Empty case passed to result display");
        return;
    }

    resultObj = *resultDesc;

    displayFrameTenant = new QLabel("Loading result data. Please Wait.",this);
    resultFrameLayout = new QHBoxLayout(ui->displayFrame);
    resultFrameLayout->addWidget(displayFrameTenant);
}

ResultVisualPopup::~ResultVisualPopup()
{
    if (displayFrameTenant != nullptr) delete displayFrameTenant;
    if (resultFrameLayout != nullptr) delete resultFrameLayout;
    delete ui;
}

void ResultVisualPopup::performStandardInit(QMap<QString, QString> neededFiles)
{
    FileNodeRef trueBaseFolder = myCase->getCaseFolder().getChildWithName(resultObj.stage);
    if (trueBaseFolder.isNil())
    {
        cwe_globals::displayPopup("ERROR: Result requested for stage that is not yet complete.");
        this->deleteLater();
        return;
    }

    setupResultDisplay(myCase->getCaseName(), myCase->getMyType()->getDisplayName(), resultObj.displayName);
    initializeWithNeededFiles(trueBaseFolder, neededFiles);
}

void ResultVisualPopup::setupResultDisplay(QString caseName, QString caseType, QString resultName)
{
    ui->label_theName->setText(caseName);
    ui->label_theType->setText(caseType);
    ui->label_theResult->setText(resultName);
    this->show();
}

void ResultVisualPopup::changeDisplayFrameTenant(QWidget * newDisplay)
{
    if (displayFrameTenant != nullptr)
    {
        displayFrameTenant->deleteLater();
    }
    displayFrameTenant = newDisplay;

    if (displayFrameTenant == nullptr) return;
    resultFrameLayout->addWidget(displayFrameTenant);
    displayFrameTenant->show();
}

void ResultVisualPopup::underlyingDataChanged(QString )
{
    //Note: This is deliberately blank. This result popup is static once the image displays.
}

RESULT_ENTRY ResultVisualPopup::getResultObj()
{
    return resultObj;
}

void ResultVisualPopup::baseFolderRemoved()
{
    QObject::disconnect(this);
    changeDisplayFrameTenant(new QLabel("Underlying case data is no longer available."));
}

void ResultVisualPopup::initialFailure()
{
    changeDisplayFrameTenant(new QLabel("Error: Data for this result not available."));
}

void ResultVisualPopup::closeButtonClicked()
{
    QObject::disconnect(this);
    this->deleteLater();
}
