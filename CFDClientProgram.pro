##################################################################################
#
# Copyright (c) 2017 The University of Notre Dame
# Copyright (c) 2017 The Regents of the University of California
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or other
# materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may
# be used to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
# SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
####################################################################################

# Contributors:
# Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CFDClientProgram
TEMPLATE = app

win32 {
    LIBS += OpenGL32.lib
} else {
    LIBS += -lz
}

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    mainWindow/cwe_mainwindow.cpp \
    visualUtils/cfdglcanvas.cpp \
    visualUtils/cfdtoken.cpp \
    visualUtils/decompresswrapper.cpp \
    cwe_guiWidgets/cwe_super.cpp \
    cwe_guiWidgets/cwe_help.cpp \
    cwe_guiWidgets/cwe_manage_simulation.cpp \
    cwe_guiWidgets/cwe_welcome_screen.cpp \
    cwe_guiWidgets/cwe_results.cpp \
    cwe_guiWidgets/cwe_parameters.cpp \
    cwe_guiWidgets/cwe_state_label.cpp \
    cwe_guiWidgets/cwe_tabwidget/cwe_tabwidget.cpp \
    cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.cpp \
    cwe_guiWidgets/cwe_tabwidget/cwe_parampanel.cpp \
    cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.cpp \
    CFDanalysis/CFDanalysisType.cpp \
    CFDanalysis/CFDcaseInstance.cpp \
    SimCenter_widgets/sctrvalidators.cpp \
    SimCenter_widgets/sctrmasterdatawidget.cpp \
    SimCenter_widgets/sctrstddatawidget.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavehandler.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavetaskguide.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavetaskreply.cpp \
    ../AgaveClientInterface/remotedatainterface.cpp \
    ../AgaveClientInterface/filemetadata.cpp \
    ../AgaveClientInterface/remotejobdata.cpp \
    ../AgaveExplorer/remoteFileOps/easyboollock.cpp \
    ../AgaveExplorer/remoteFileOps/fileoperator.cpp \
    ../AgaveExplorer/remoteFileOps/filetreenode.cpp \
    ../AgaveExplorer/remoteFileOps/joboperator.cpp \
    ../AgaveExplorer/SimCenterCommon/FooterWidget.cpp \
    ../AgaveExplorer/SimCenterCommon/HeaderWidget.cpp \
    ../AgaveExplorer/utilFuncs/agavesetupdriver.cpp \
    ../AgaveExplorer/utilFuncs/authform.cpp \
    ../AgaveExplorer/utilFuncs/copyrightdialog.cpp \
    ../AgaveExplorer/utilFuncs/singlelinedialog.cpp \
    SimCenter_widgets/sctrbooldatawidget.cpp \
    SimCenter_widgets/sctrchoicedatawidget.cpp \
    SimCenter_widgets/sctrfiledatawidget.cpp \
    cwe_globals.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavepipebuffer.cpp \
    cwe_guiWidgets/cwe_job_list.cpp \
    ../AgaveExplorer/remoteFileOps/joblistnode.cpp \
    ../AgaveExplorer/ae_globals.cpp \
    cwe_interfacedriver.cpp \
    visualUtils/resultVisuals/resultmesh2dwindow.cpp \
    visualUtils/resultprocurebase.cpp \
    visualUtils/resultvisualpopup.cpp \
    visualUtils/resultVisuals/resultfield2dwindow.cpp \
    visualUtils/resultVisuals/resulttextdisp.cpp \
    cwe_guiWidgets/cwe_file_manager.cpp \
    CFDanalysis/cwejobaccountant.cpp \
    ../AgaveExplorer/remoteFileOps/filenoderef.cpp \
    ../AgaveExplorer/remoteModelViews/linkedstandarditem.cpp \
    ../AgaveExplorer/remoteModelViews/remotefilemodel.cpp \
    ../AgaveExplorer/remoteModelViews/remotefiletree.cpp \
    ../AgaveExplorer/remoteModelViews/remotejoblister.cpp \
    popupWindows/create_case_popup.cpp \
    popupWindows/duplicate_case_popup.cpp \
    ../AgaveExplorer/remoteModelViews/remotefileitem.cpp \
    SimCenter_widgets/sctrtextdatawidget.cpp \
    popupWindows/cwe_popup.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavethread.cpp

HEADERS  += \
    CFDanalysis/CFDanalysisType.h \
    CFDanalysis/CFDcaseInstance.h \
    visualUtils/cfdglcanvas.h \
    visualUtils/cfdtoken.h \
    visualUtils/decompresswrapper.h \
    mainWindow/cwe_mainwindow.h \
    cwe_guiWidgets/cwe_super.h \
    cwe_guiWidgets/cwe_help.h \
    cwe_guiWidgets/cwe_manage_simulation.h \
    cwe_guiWidgets/cwe_welcome_screen.h \
    cwe_guiWidgets/cwe_results.h \
    cwe_guiWidgets/cwe_parameters.h \
    cwe_guiWidgets/cwe_state_label.h \
    cwe_guiWidgets/cwe_tabwidget/cwe_tabwidget.h \
    cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.h \
    cwe_guiWidgets/cwe_tabwidget/cwe_parampanel.h \
    cwe_guiWidgets/cwe_tabwidget/cwe_groupswidget.h \
    SimCenter_widgets/sctrvalidators.h \
    SimCenter_widgets/sctrmasterdatawidget.h \
    SimCenter_widgets/sctrstddatawidget.h \
    SimCenter_widgets/sctrstates.h \
    ../AgaveClientInterface/agaveInterfaces/agavehandler.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskguide.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskreply.h \
    ../AgaveClientInterface/remotedatainterface.h \
    ../AgaveClientInterface/filemetadata.h \
    ../AgaveClientInterface/remotejobdata.h \
    ../AgaveExplorer/remoteFileOps/easyboollock.h \
    ../AgaveExplorer/remoteFileOps/fileoperator.h \
    ../AgaveExplorer/remoteFileOps/filetreenode.h \
    ../AgaveExplorer/remoteFileOps/joboperator.h \
    ../AgaveExplorer/SimCenterCommon/FooterWidget.h \
    ../AgaveExplorer/SimCenterCommon/HeaderWidget.h \
    ../AgaveExplorer/utilFuncs/agavesetupdriver.h \
    ../AgaveExplorer/utilFuncs/authform.h \
    ../AgaveExplorer/utilFuncs/copyrightdialog.h \
    ../AgaveExplorer/utilFuncs/singlelinedialog.h \
    SimCenter_widgets/sctrbooldatawidget.h \
    SimCenter_widgets/sctrchoicedatawidget.h \
    SimCenter_widgets/sctrfiledatawidget.h \
    cwe_globals.h \
    ../AgaveClientInterface/agaveInterfaces/agavepipebuffer.h \
    cwe_guiWidgets/cwe_job_list.h \
    ../AgaveExplorer/remoteFileOps/joblistnode.h \
    ../AgaveExplorer/ae_globals.h \
    cwe_interfacedriver.h \
    visualUtils/resultVisuals/resultmesh2dwindow.h \
    visualUtils/resultprocurebase.h \
    visualUtils/resultvisualpopup.h \
    visualUtils/resultVisuals/resultfield2dwindow.h \
    visualUtils/resultVisuals/resulttextdisp.h \
    cwe_guiWidgets/cwe_file_manager.h \
    CFDanalysis/cwejobaccountant.h \
    ../AgaveExplorer/remoteFileOps/filenoderef.h \
    ../AgaveExplorer/remoteModelViews/linkedstandarditem.h \
    ../AgaveExplorer/remoteModelViews/remotefilemodel.h \
    ../AgaveExplorer/remoteModelViews/remotefiletree.h \
    ../AgaveExplorer/remoteModelViews/remotejoblister.h \
    popupWindows/create_case_popup.h \
    popupWindows/duplicate_case_popup.h \
    ../AgaveExplorer/remoteModelViews/remotefileitem.h \
    SimCenter_widgets/sctrtextdatawidget.h \
    popupWindows/cwe_popup.h \
    ../AgaveClientInterface/agaveInterfaces/agavethread.h

FORMS    += \
    mainWindow/cwe_mainwindow.ui \
    cwe_guiWidgets/cwe_help.ui \
    cwe_guiWidgets/cwe_manage_simulation.ui \
    cwe_guiWidgets/cwe_welcome_screen.ui \
    cwe_guiWidgets/cwe_results.ui \
    cwe_guiWidgets/cwe_parameters.ui \
    cwe_guiWidgets/cwe_debug_widget.ui \
    cwe_guiWidgets/cwe_tabwidget/cwe_tabwidget.ui \
    ../AgaveExplorer/utilFuncs/authform.ui \
    ../AgaveExplorer/utilFuncs/copyrightdialog.ui \
    ../AgaveExplorer/utilFuncs/singlelinedialog.ui \
    cwe_guiWidgets/cwe_tabwidget/cwe_stagestatustab.ui \
    cwe_guiWidgets/cwe_tabwidget/cwe_parampanel.ui \
    cwe_guiWidgets/cwe_job_list.ui \
    visualUtils/resultvisualpopup.ui \
    cwe_guiWidgets/cwe_file_manager.ui \
    popupWindows/create_case_popup.ui \
    popupWindows/duplicate_case_popup.ui

RESOURCES += \
    cwe_resources.qrc \
    ../AgaveExplorer/SimCenterCommon/commonResources.qrc \
    CFDanalysis/config/cfdconfig.qrc

mkdir_help_resources.commands = $(MKDIR) $$OUT_PWD/resources
create_help_resources.commands = rcc -binary $$PWD/cwe_help.qrc -o $$OUT_PWD/resources/cwe_help.rcc
create_help_resources.depends = mkdir_help_resources
first.depends += create_help_resources
export(first.depends)
export(create_help_resources.depends)
export(create_help_resources.commands)
export(mkdir_help_resources.commands)
QMAKE_EXTRA_TARGETS += first create_help_resources mkdir_help_resources
