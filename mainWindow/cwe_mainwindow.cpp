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

#include "cwe_guiWidgets/cwe_param_tabs/cwe_paneltab.h"

#include "../AgaveExplorer/utilFuncs/copyrightdialog.h"
#include "cwe_interfacedriver.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "cwe_globals.h"

#include "utilWindows/dialogabout.h"

CWE_MainWindow::CWE_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CWE_MainWindow)
{
    ui->setupUi(this);

    //Set Header text
    ui->header->setHeadingText("SimCenter CWE Workbench");

    addWindowPanel(ui->tab_welcome_screen,"welcome","Welcome");
    addWindowPanel(ui->tab_help,"help","Help");
    ui->panelTabsLayout->addItem(new QSpacerItem(150, 0));
    addWindowPanel(ui->tab_manage_and_run,"manage","Simulation Cases");
    addWindowPanel(ui->tab_jobs_page,"jobs","Remote Jobs");
    addWindowPanel(ui->tab_files,"files","Remote Files");
    ui->panelTabsLayout->addItem(new QSpacerItem(150, 0));
    addWindowPanel(ui->tab_parameters,"parameters","Parameters");
    addWindowPanel(ui->tab_results,"results","Results");

    changeParamsAndResultsEnabled(false);

    // adjust application size to display
    QRect rec = QGuiApplication::screens().at(0)->availableGeometry();
    int height = static_cast<int>(this->height()>0.75*rec.height()?this->height():0.75*rec.height());
    if ( height > 0.95*rec.height() ) { height = static_cast<int>(0.95*rec.height()); }
    int width  = static_cast<int>(this->width()>0.65*rec.width()?this->width():0.65*rec.width());
    if ( width > 0.95*rec.width() ) { width = static_cast<int>(0.95*rec.width()); }
    this->resize(width, height);
}

CWE_MainWindow::~CWE_MainWindow()
{
    delete ui;
}

void CWE_MainWindow::addWindowPanel(QWidget * thePanel, QString panelName, QString tabText)
{
    CWE_PanelTab * panelPtr = new CWE_PanelTab(thePanel,panelName,tabText, this);
    listOfPanelTabs.insert(thePanel, panelPtr);
    ui->panelTabsLayout->addWidget(panelPtr);
    QObject::connect(panelPtr, SIGNAL(btn_clicked(CWE_ParamTab*)), this, SLOT(panelTabClicked(CWE_ParamTab*)));
}

void CWE_MainWindow::runSetupSteps()
{
    //Note: Adding widget to header will re-parent them
    stateLabel = new cwe_state_label(this);
    stateLabel->setAlignment(Qt::AlignHCenter);
    stateLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    ui->header->appendWidget(stateLabel);

    QLabel * username = new QLabel(cwe_globals::get_connection()->getUserName());
    ui->header->appendWidget(username);

    QPushButton * logoutButton = new QPushButton("Logout");
    logoutButton->setObjectName("logoutButton");
    QObject::connect(logoutButton, SIGNAL(clicked(bool)), cwe_globals::get_CWE_Driver(), SLOT(shutdown()));
    ui->header->appendWidget(logoutButton);

    for (int i = 0; i < ui->tab_panel_stack->count(); i++)
    {
        QWidget * rawWidget = ui->tab_panel_stack->widget(i);
        if (!rawWidget->inherits("CWE_Super"))
        {
            continue;
        }
        CWE_Super * aWidget = qobject_cast<CWE_Super *>(rawWidget);
        aWidget->linkMainWindow(this);
    }

    setCurrentPanel(ui->tab_welcome_screen);
}

void CWE_MainWindow::newCaseState(CaseState newState)
{
    QObject * theSender = sender();
    if (theSender != nullptr)
    {
        CFDcaseInstance * theCase = qobject_cast<CFDcaseInstance *>(theSender);
        if (theCase != currentCase) return;
    }

    if ((newState == CaseState::DEFUNCT) ||
            (newState == CaseState::ERROR) ||
            (newState == CaseState::INVALID))
    {
        changeParamsAndResultsEnabled(false);
    }
    else if (newState == CaseState::LOADING)
    {
        return;
    }
    else
    {
        changeParamsAndResultsEnabled(true);
    }
}

void CWE_MainWindow::menuCopyInfo()
{
    CopyrightDialog copyrightPopup;
    copyrightPopup.exec();
}

void CWE_MainWindow::menuExit()
{
    cwe_globals::get_CWE_Driver()->shutdown();
}

void CWE_MainWindow::panelTabClicked(CWE_ParamTab * newTab)
{
    CWE_PanelTab * thePanel = qobject_cast<CWE_PanelTab *>(newTab);
    if (thePanel == nullptr)
    {
        cwe_globals::displayFatalPopup("UI Error, unable to find main window panel", "Internal Error");
    }

    if (thePanel->tabIsActive()) return;
    CWE_Super * panelWidget = qobject_cast<CWE_Super *>(ui->tab_panel_stack->currentWidget());
    if (panelWidget == nullptr)
    {
        cwe_globals::displayFatalPopup("UI Error, unable connet main window panel", "Internal Error");
    }
    if (!panelWidget->allowClickAway()) return;

    setCurrentPanel(thePanel->getPanelWidget());
}

void CWE_MainWindow::switchToHelpTab(const QUrl &url)
{
    qDebug() << "CWE_MainWindow::switchToHelpTab(" << url.toString() << ")";

    setCurrentPanel(ui->tab_help);
    ui->tab_help->setPageSource(url);
}

void CWE_MainWindow::switchToResultsTab()
{
    if (listOfPanelTabs.value(ui->tab_results)->isEnabled())
    {
        setCurrentPanel(ui->tab_results);
    }
    else
    {
        setCurrentPanel(ui->tab_manage_and_run);
    }
}

void CWE_MainWindow::switchToParameterTab()
{
    if (listOfPanelTabs.value(ui->tab_parameters)->isEnabled())
    {
        setCurrentPanel(ui->tab_parameters);
    }
    else
    {
        setCurrentPanel(ui->tab_manage_and_run);
    }
}

void CWE_MainWindow::switchToFilesTab()
{
    setCurrentPanel(ui->tab_files);
}

RemoteFileModel * CWE_MainWindow::getFileModel()
{
    return &fileModel;
}

void CWE_MainWindow::changeParamsAndResultsEnabled(bool setting)
{
    if (setting == true)
    {
        changeTabEnabled(ui->tab_parameters, true);
        changeTabEnabled(ui->tab_results, true);

        return;
    }

    if (panelIsActive(ui->tab_parameters) || panelIsActive(ui->tab_results))
    {
        setCurrentPanel(ui->tab_manage_and_run);
    }

    changeTabEnabled(ui->tab_parameters, false);
    changeTabEnabled(ui->tab_results, false);
}

void CWE_MainWindow::changeTabEnabled(QWidget * thePanel, bool newSetting)
{
    //TODO: Want these visible but disabled when disabled
    listOfPanelTabs.value(thePanel)->setTabEnabled(newSetting);
}

CFDcaseInstance * CWE_MainWindow::getCurrentCase()
{
    return currentCase;
}

void CWE_MainWindow::setCurrentCase()
{
    if (currentCase == nullptr) return;

    deactivateCurrentCase();
    currentCase = nullptr;
    stateLabel->setCurrentCase(currentCase);
    emit haveNewCase();
}

void CWE_MainWindow::setCurrentCase(CFDcaseInstance * newCase)
{
    if (newCase == currentCase) return;

    changeParamsAndResultsEnabled(false);
    stateLabel->setCurrentCase(newCase);

    deactivateCurrentCase();
    currentCase = newCase;
    if (currentCase == nullptr) return;

    QObject::connect(currentCase, SIGNAL(haveNewState(CaseState)),
                    this, SLOT(newCaseState(CaseState)),
                    Qt::QueuedConnection);
    //Manually invoke state change to initialize visibility
    newCaseState(currentCase->getCaseState());
    emit haveNewCase();
}

void CWE_MainWindow::setCurrentCase(const FileNodeRef &caseNode)
{
    CFDcaseInstance * newCase = getCaseFromFolder(caseNode);
    if (newCase == nullptr)
    {
        setCurrentCase();
        return;
    }
    setCurrentCase(newCase);
}

void CWE_MainWindow::setCurrentCase(CFDanalysisType * newCaseType)
{
    CFDcaseInstance * newCase = getCaseFromType(newCaseType);
    if (newCase == nullptr)
    {
        setCurrentCase();
        return;
    }
    setCurrentCase(newCase);
}

CFDcaseInstance * CWE_MainWindow::getCaseFromFolder(const FileNodeRef &caseNode)
{
    if ((currentCase != nullptr) && (currentCase->getCaseFolder().getFullPath() == caseNode.getFullPath()))
    {
        return currentCase;
    }

    if (caseNode.getFileType() != FileType::DIR)
    {
        return nullptr;
    }

    CFDcaseInstance * newCase = new CFDcaseInstance(caseNode);
    return newCase;
}

CFDcaseInstance * CWE_MainWindow::getCaseFromType(CFDanalysisType * caseType)
{
    CFDcaseInstance * ret;
    if (caseType == nullptr)
    {
        ret = new CFDcaseInstance();
    }
    else
    {
        ret = new CFDcaseInstance(caseType);
    }
    return ret;
}

void CWE_MainWindow::deactivateCurrentCase()
{
    if (currentCase == nullptr) return;
    QObject::disconnect(currentCase,nullptr,nullptr,nullptr);
    currentCase->deleteLater();
    currentCase = nullptr;
}

bool CWE_MainWindow::panelIsActive(QWidget * panelToCheck)
{
    return listOfPanelTabs.value(panelToCheck)->tabIsActive();
}

void CWE_MainWindow::setCurrentPanel(QWidget *newActivePanel)
{
    for (CWE_PanelTab * aTab : listOfPanelTabs)
    {
        if (aTab->getPanelWidget() == newActivePanel)
        {
            aTab->setActive();
            ui->tab_panel_stack->setCurrentWidget(newActivePanel);
        }
        else
        {
            aTab->setInActive();
        }
    }
}

void CWE_MainWindow::on_actionAbout_CWE_triggered()
{
    DialogAbout *dlg = new DialogAbout();

    //
    // adjust size of application window to the available display
    //
    QRect rec = QGuiApplication::screens().at(0)->availableGeometry();
    int height = static_cast<int>(0.50*rec.height());
    int width  = static_cast<int>(0.50*rec.width());
    dlg->resize(width, height);

    dlg->exec();
    delete dlg;
}
