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

#include "cwe_manage_simulation.h"
#include "ui_cwe_manage_simulation.h"
#include "cwe_defines.h"

#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "CFDanalysis/CFDanalysisType.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "vwtinterfacedriver.h"

CWE_manage_simulation::CWE_manage_simulation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_manage_simulation)
{
    ui->setupUi(this);

    QObject::connect(ui->treeView, SIGNAL(newFileSelected(FileTreeNode*)),
                             this, SLOT(newFileSelected(FileTreeNode*)));
}

CWE_manage_simulation::~CWE_manage_simulation()
{
    delete ui;
}

void CWE_manage_simulation::linkDriver(VWTinterfaceDriver * theDriver)
{
    driverLink = theDriver;
}

void CWE_manage_simulation::newFileSelected(FileTreeNode * newFile)
{
    CFDcaseInstance * newCase = new CFDcaseInstance(newFile, driverLink);
    driverLink->setCurrentCase(newCase);

    emit needParamTab();
}

void CWE_manage_simulation::on_pb_viewParameters_clicked()
{
    // switch main window to parameters tab
}

void CWE_manage_simulation::on_pb_viewResults_clicked()
{
    // switch main window to results tab
}
