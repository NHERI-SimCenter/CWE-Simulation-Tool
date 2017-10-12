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

#include "vwtinterfacedriver.h"

#include "../AgaveClientInterface/agaveInterfaces/agavehandler.h"

#include "../AgaveExplorer/utilFuncs/authform.h"

#include "../AgaveExplorer/remoteFileOps/joboperator.h"
#include "../AgaveExplorer/remoteFileOps/fileoperator.h"

#include "CFDanalysis/CFDcaseInstance.h"
#include "CFDanalysis/CFDanalysisType.h"

#include "mainWindow/cwe_mainwindow.h"

VWTinterfaceDriver::VWTinterfaceDriver(QObject *parent) : AgaveSetupDriver(parent)
{
    AgaveHandler * tmpHandle = new AgaveHandler(this);
    tmpHandle->registerAgaveAppInfo("compress", "compress-0.1u1",{"directory", "compression_type"},{},"directory");
    tmpHandle->registerAgaveAppInfo("extract", "extract-0.1u1",{"inputFile"},{},"");
    tmpHandle->registerAgaveAppInfo("openfoam","openfoam-2.4.0u11",{"solver"},{"inputDirectory"},"inputDirectory");

    //The following are being debuged:
    tmpHandle->registerAgaveAppInfo("FileEcho", "fileEcho-0.1.0",{"directory","NewFile", "EchoText"},{},"directory");
    tmpHandle->registerAgaveAppInfo("cwe-create", "cwe-create-0.1.0", {"directory", "newFolder", "template"}, {}, "directory");
    tmpHandle->registerAgaveAppInfo("cwe-update", "cwe-update-0.1.0", {"directory", "params"}, {}, "directory");
    tmpHandle->registerAgaveAppInfo("cwe-exec-serial", "cwe-exec-serial-0.1.0", {"directory", "action", "infile"}, {}, "directory");
    tmpHandle->registerAgaveAppInfo("cwe-sim", "cwe-sim-2.4.0", {}, {"solver", "directory"}, "directory");
    tmpHandle->registerAgaveAppInfo("cwe-delete", "cwe-delete-0.1.0", {"directory", "step"}, {}, "directory");

    theConnector = (RemoteDataInterface *) tmpHandle;
    QObject::connect(theConnector, SIGNAL(sendFatalErrorMessage(QString)), this, SLOT(fatalInterfaceError(QString)));

    /* populate tab_NewCase with available cases */
    QDir confDir(":/config");
    QStringList filters;
    filters << "*.json" << "*.JSON";
    QStringList caseTypeFiles = confDir.entryList(filters);

    foreach (QString caseConfigFile, caseTypeFiles) {
        QString confPath = ":/config/";
        confPath = confPath.append(caseConfigFile);
        CFDanalysisType * newTemplate = new CFDanalysisType(confPath);
        templateList.append(newTemplate);
    }
}

void VWTinterfaceDriver::startup()
{
    authWindow = new AuthForm(this);
    authWindow->show();
    QObject::connect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    mainWindow = new CWE_MainWindow(this);
}

void VWTinterfaceDriver::closeAuthScreen()
{
    if (mainWindow == NULL)
    {
        fatalInterfaceError("Fatal Error: Main window not found");
    }

    myJobHandle = new JobOperator(theConnector,this);
    myJobHandle->demandJobDataRefresh();
    myFileHandle = new FileOperator(theConnector,this);
    myFileHandle->resetFileData();

    mainWindow->runSetupSteps();
    mainWindow->show();

    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    if (authWindow != NULL)
    {
        QObject::disconnect(authWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
        authWindow->hide();
        authWindow->deleteLater();
        authWindow = NULL;
    }
}

void VWTinterfaceDriver::startOffline()
{
    mainWindow = new CWE_MainWindow(this);

    myJobHandle = new JobOperator(theConnector,this);
    myFileHandle = new FileOperator(theConnector,this);

    setCurrentCase(new CFDcaseInstance(templateList.at(0),this));

    mainWindow->runOfflineSetupSteps();
    mainWindow->show();

    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
}

QString VWTinterfaceDriver::getBanner()
{
    return "SimCenter CWE CFD Client Program";
}

QString VWTinterfaceDriver::getVersion()
{
    return "Version: 0.1";
}

QList<CFDanalysisType *> * VWTinterfaceDriver::getTemplateList()
{
    return &templateList;
}

CFDcaseInstance * VWTinterfaceDriver::getCurrentCase()
{
    return currentCFDCase;
}

void VWTinterfaceDriver::setCurrentCase(CFDcaseInstance * newCase)
{
    if (newCase == currentCFDCase) return;

    CFDcaseInstance * oldCase = currentCFDCase;
    currentCFDCase = newCase;

    if (oldCase != NULL)
    {
        QObject::disconnect(oldCase,0,0,0);
        if (!oldCase->isDefunct())
        {
            oldCase->killCaseConnection();
        }
    }

    if (newCase != NULL)
    {
        QObject::connect(newCase, SIGNAL(detachCase()),
                         this, SLOT(currentCaseInvalidated()));
        mainWindow->attachCaseSignals(newCase);
    }
    emit haveNewCase();
}

void VWTinterfaceDriver::currentCaseInvalidated()
{
    setCurrentCase(NULL);
}
