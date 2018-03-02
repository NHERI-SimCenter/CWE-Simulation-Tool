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
#include "../AgaveClientInterface/agaveInterfaces/agavetaskreply.h"
#include "../AgaveClientInterface/remotejobdata.h"

#include "../AgaveExplorer/utilFuncs/authform.h"

#include "../AgaveExplorer/remoteFileOps/joboperator.h"
#include "../AgaveExplorer/remoteFileOps/fileoperator.h"

#include "CFDanalysis/CFDcaseInstance.h"
#include "CFDanalysis/CFDanalysisType.h"

#include "mainWindow/cwe_mainwindow.h"

VWTinterfaceDriver::VWTinterfaceDriver(QObject *parent, bool debug) : AgaveSetupDriver(parent, debug)
{
    AgaveHandler * tmpHandle = new AgaveHandler(this);
    tmpHandle->registerAgaveAppInfo("compress", "compress-0.1u1",{"directory", "compression_type"},{},"directory");
    tmpHandle->registerAgaveAppInfo("extract", "extract-0.1u1",{"inputFile"},{},"");

    tmpHandle->registerAgaveAppInfo("cwe-serial", "cwe-serial-0.1.0", {"stage"}, {"directory", "file_input"}, "directory");
    tmpHandle->registerAgaveAppInfo("cwe-parallel", "cwe-parallel-0.1.0", {"stage"}, {"directory", "file_input"}, "directory");

    theConnector = (RemoteDataInterface *) tmpHandle;
    QObject::connect(theConnector, SIGNAL(sendFatalErrorMessage(QString)), this, SLOT(fatalInterfaceError(QString)));

    /* populate with available cases */
    QDir confDir(":/config");
    QStringList filters;
    filters << "*.json" << "*.JSON";
    QStringList caseTypeFiles = confDir.entryList(filters);

    foreach (QString caseConfigFile, caseTypeFiles) {
        QString confPath = ":/config/";
        confPath = confPath.append(caseConfigFile);
        CFDanalysisType * newTemplate = new CFDanalysisType(confPath);
        if (debug == false)
        {
            if (newTemplate->isDebugOnly() == false)
            {
                templateList.append(newTemplate);
            }
        }
        else
        {
            templateList.append(newTemplate);
        }
    }
}

VWTinterfaceDriver::~VWTinterfaceDriver()
{
    if (mainWindow != NULL)
    {
        delete mainWindow;
        mainWindow = NULL;
    }
    if (authWindow != NULL)
    {
        delete authWindow;
        authWindow = NULL;
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
    myFileHandle = new FileOperator(theConnector,this);

    myJobHandle->demandJobDataRefresh();
    QObject::connect(myJobHandle, SIGNAL(newJobData()), this, SLOT(processNewJobInfo()));

    myFileHandle->resetFileData();

    mainWindow->runSetupSteps();
    mainWindow->show();

    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));

    AgaveHandler * tmpHandle = (AgaveHandler *) theConnector;
    AgaveTaskReply * getAppList = tmpHandle->getAgaveAppList();

    if (getAppList == NULL)
    {
        fatalInterfaceError("Unable to get app list from DesignSafe");
        return;
    }
    QObject::connect(getAppList, SIGNAL(haveAgaveAppList(RequestState,QJsonArray*)),
                     this, SLOT(checkAppList(RequestState,QJsonArray*)));

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
    offlineMode = true;

    mainWindow = new CWE_MainWindow(this);

    myJobHandle = new JobOperator(theConnector,this);
    myFileHandle = new FileOperator(theConnector,this);

    setCurrentCase(new CFDcaseInstance(templateList.at(0),this));

    mainWindow->runSetupSteps();
    mainWindow->show();

    QObject::connect(mainWindow->windowHandle(),SIGNAL(visibleChanged(bool)),this, SLOT(subWindowHidden(bool)));
}

QString VWTinterfaceDriver::getBanner()
{
    return "SimCenter CWE CFD Client Program";
}

QString VWTinterfaceDriver::getVersion()
{
    return "Version: 0.3.0";
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
    }
    emit haveNewCase();
}

CWE_MainWindow * VWTinterfaceDriver::getMainWindow()
{
    return mainWindow;
}

void VWTinterfaceDriver::currentCaseInvalidated()
{
    setCurrentCase(NULL);
}

void VWTinterfaceDriver::checkAppList(RequestState replyState, QJsonArray * appList)
{
    if (replyState != RequestState::GOOD)
    {
        fatalInterfaceError("Unable to connect to Agave to get app info.");
        return;
    }

    QList<QString> neededApps = {"cwe-serial", "cwe-parallel"};

    for (auto itr = appList->constBegin(); itr != appList->constEnd(); itr++)
    {
        QString appName = (*itr).toObject().value("name").toString();

        if (appName.isEmpty())
        {
            continue;
        }
        if (neededApps.contains(appName))
        {
            neededApps.removeAll(appName);
        }
    }

    if (!neededApps.isEmpty())
    {
        fatalInterfaceError("The CWE program depends on several apps hosted on DesignSafe which are not public. Please contact the SimCenter project to be able to access these apps.");
    }
}

void VWTinterfaceDriver::processNewJobInfo()
{
    QMap<QString, RemoteJobData> runningJobs = getRunningCWEjobs();
    for (auto itr = runningJobs.cbegin(); itr != runningJobs.cend(); itr++)
    {
        if (!(*itr).detailsLoaded())
        {
            getJobHandler()->requestJobDetails(*itr);
        }
    }
}

QMap<QString, RemoteJobData> VWTinterfaceDriver::getRunningCWEjobs()
{
    QMap<QString, RemoteJobData> ret;
    QMap<QString, RemoteJobData> runningJobs = getJobHandler()->getRunningJobs();
    for (auto itr = runningJobs.cbegin(); itr != runningJobs.cend(); itr++)
    {
        QString theApp = (*itr).getApp();
        if ((theApp == "cwe-serial") || (theApp == "cwe-parallel"))
        {
            ret.insert(itr.key(),*itr);
        }
    }
    return ret;
}

bool VWTinterfaceDriver::inOfflineMode()
{
    return offlineMode;
}
