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

#include "resultmesh2dwindow.h"

#include "CFDanalysis/CFDcaseInstance.h"
#include "CFDanalysis/CFDanalysisType.h"
#include "cwe_globals.h"
#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "../cfdglcanvas.h"

ResultMesh2dWindow::ResultMesh2dWindow(CFDcaseInstance * theCase, QMap<QString, QString> resultDesc, QWidget *parent):
    ResultVisualPopup(parent)
{
    myCase = theCase;
    if (myCase == NULL)
    {
        cwe_globals::displayFatalPopup("Internal error: Empty case passed to result object");
        return;
    }

    resultObj = resultDesc;

    loadingLabel = new QLabel("Loading result data. Please Wait.",this);
    resultFrameLayout = new QHBoxLayout(this->getDisplayFrame());
    resultFrameLayout->addWidget(loadingLabel);
}

ResultMesh2dWindow::~ResultMesh2dWindow()
{
    if (loadingLabel != NULL) delete loadingLabel;
    if (resultFrameLayout != NULL) delete resultFrameLayout;
    for (auto itr = fileBuffers.cbegin(); itr != fileBuffers.cend(); itr++)
    {
        delete (*itr);
    }
}

void ResultMesh2dWindow::initializeView()
{
    QMap<QString, QString> neededFiles;
    neededFiles["points"] = "/constant/polyMesh/points.gz";
    neededFiles["faces"] = "/constant/polyMesh/faces.gz";
    neededFiles["owner"] = "/constant/polyMesh/owner.gz";

    FileTreeNode * trueBaseFolder = myCase->getCaseFolderNode()->getChildNodeWithName(resultObj["stage"]);
    if (trueBaseFolder == NULL)
    {
        cwe_globals::displayPopup("ERROR: Result requested for stage that is not yet complete.");
        this->deleteLater();
        return;
    }

    initializeWithNeededFiles(trueBaseFolder, neededFiles);
    setupResultDisplay(myCase->getCaseName(), myCase->getMyType()->getName(), "2D Mesh");
}

void ResultMesh2dWindow::allFilesLoaded()
{
    QObject::disconnect(this);
    fileBuffers = getFileBuffers();

    if (myCanvas != NULL) myCanvas->deleteLater();
    myCanvas = new CFDglCanvas();
    myCanvas->loadMeshData(fileBuffers["points"], fileBuffers["faces"], fileBuffers["owner"]);

    if (!myCanvas->haveMeshData())
    {
        loadingLabel->setText("Error: Data for 2D mesh result is unreadable. Please reset and try again.");
    }

    loadingLabel->deleteLater();
    loadingLabel = NULL;
    getDisplayFrame()->layout()->addWidget(myCanvas);

    myCanvas->setDisplayState(CFDDisplayState::MESH);
    myCanvas->show();
}

void ResultMesh2dWindow::underlyingDataChanged(FileTreeNode * , bool)
{
    //Note: This is deliberately blank. This result popup is static once the image displays.
}

void ResultMesh2dWindow::initialFailure()
{
    loadingLabel->setText("Error: Data for this result not available.");
}
