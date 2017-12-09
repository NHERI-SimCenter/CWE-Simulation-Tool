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

#include "cwe_mainwindow.h"
#include "ui_cwe_mainwindow.h"

#include "CFDanalysis/CFDcaseInstance.h"

#include "cwe_guiWidgets/cwe_state_label.h"

#include <QDesktopWidget>
#include <QDebug>

CWE_MainWindow::CWE_MainWindow(VWTinterfaceDriver *newDriver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWE_MainWindow)
{
    ui->setupUi(this);

    //Esablish connections with driver
    myDriver = newDriver;

    if (!myDriver->inDebugMode())
    {
        ui->tab_debug->deleteLater();
    }

    //Set Header text
    ui->header->setHeadingText("SimCenter CWE Workbench");

    changeTabVisible((QTabWidget *)ui->tab_spacer_1, false);
    changeTabVisible((QTabWidget *)ui->tab_spacer_2, false);

    // adjust application size to display
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = this->height()<0.5*rec.height()?this->height():0.5*rec.height();
    int width  = this->width()<0.5*rec.width()?this->width():0.5*rec.width();
    this->resize(width, height);
}

CWE_MainWindow::~CWE_MainWindow()
{
    delete ui;
}

void CWE_MainWindow::runSetupSteps()
{
    //Note: Adding widget to header will re-parent them
    stateLabel = new cwe_state_label(this);
    ui->header->appendWidget(stateLabel);

    QLabel * username = new QLabel(myDriver->getDataConnection()->getUserName());
    ui->header->appendWidget(username);

    QPushButton * logoutButton = new QPushButton("Logout");
    QObject::connect(logoutButton, SIGNAL(clicked(bool)), myDriver, SLOT(shutdown()));
    ui->header->appendWidget(logoutButton);

    for (int i = 0; i < ui->tabContainer->count(); i++)
    {
        QWidget * rawWidget = ui->tabContainer->widget(i);
        if (!rawWidget->inherits("CWE_Super"))
        {
            continue;
        }
        CWE_Super * aWidget = (CWE_Super *) rawWidget;
        aWidget->linkDriver(myDriver);
    }
}

void CWE_MainWindow::attachCaseSignals(CFDcaseInstance * newCase)
{
    QObject::connect(newCase, SIGNAL(haveNewState(CaseState)),
                     ui->tab_parameters, SLOT(newCaseState(CaseState)));
    if (stateLabel != NULL)
    {
        stateLabel->setCurrentCase(newCase);
    }
    //It is expected that this list will grow
}

void CWE_MainWindow::menuCopyInfo()
{
    CopyrightDialog copyrightPopup;
    copyrightPopup.exec();
}

void CWE_MainWindow::menuExit()
{
    myDriver->shutdown();
}

void CWE_MainWindow::on_action_Quit_triggered()
{
    myDriver->shutdown();
}

void CWE_MainWindow::on_actionOpen_triggered()
{

}

void CWE_MainWindow::on_actionOpen_existing_triggered()
{

}

void CWE_MainWindow::on_actionSave_triggered()
{

}

void CWE_MainWindow::on_actionSave_As_triggered()
{

}

void CWE_MainWindow::on_actionAbout_CWE_triggered()
{

}

void CWE_MainWindow::on_actionCreate_New_Simulation_triggered()
{

}

void CWE_MainWindow::on_actionManage_Simulation_triggered()
{

}

void CWE_MainWindow::on_actionHelp_triggered()
{

}

void CWE_MainWindow::on_action_Landing_Page_triggered()
{

}

void CWE_MainWindow::on_actionManage_Remote_Jobs_triggered()
{

}

void CWE_MainWindow::on_actionTutorials_and_Help_triggered()
{

}

void CWE_MainWindow::on_actionManage_and_Download_Files_triggered()
{

}

void CWE_MainWindow::switchToResultsTab()
{
    ui->tabContainer->setCurrentWidget(ui->tab_results);
}

void CWE_MainWindow::switchToParameterTab()
{
    ui->tabContainer->setCurrentWidget(ui->tab_parameters);
}

void CWE_MainWindow::switchToCreateTab()
{
    ui->tabContainer->setCurrentWidget(ui->tab_create_new);
}

void CWE_MainWindow::changeTabVisible(QTabWidget * theTab, bool newSetting)
{
    ui->tabContainer->setTabEnabled(ui->tabContainer->indexOf(theTab),newSetting);
}
