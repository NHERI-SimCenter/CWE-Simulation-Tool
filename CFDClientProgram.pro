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

QT       += core gui network

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
    vwtinterfacedriver.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavehandler.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavetaskguide.cpp \
    ../AgaveClientInterface/agaveInterfaces/agavetaskreply.cpp \
    ../AgaveClientInterface/remotedatainterface.cpp \
    ../AgaveClientInterface/filemetadata.cpp \
    ../AgaveClientInterface/remotejobdata.cpp \
    visualUtils/cfdglcanvas.cpp \
    visualUtils/cfdtoken.cpp \
    visualUtils/decompresswrapper.cpp \
    mainWindow/cwe_mainwindow.cpp \
    cwe_guiWidgets/cwe_create_simulation.cpp \
    cwe_guiWidgets/cwe_file_manager.cpp \
    cwe_guiWidgets/cwe_help.cpp \
    cwe_guiWidgets/cwe_landing.cpp \
    cwe_guiWidgets/cwe_manage_simulation.cpp \
    cwe_guiWidgets/cwe_simulation_details.cpp \
    cwe_guiWidgets/cwe_task_list.cpp \
    cwe_guiWidgets/sidebar.cpp \
    analysistypes/baseanalysistype.cpp \
    ../AgaveExplorer/remoteFileOps/easyboollock.cpp \
    ../AgaveExplorer/remoteFileOps/fileoperator.cpp \
    ../AgaveExplorer/remoteFileOps/filetreenode.cpp \
    ../AgaveExplorer/remoteFileOps/joboperator.cpp \
    ../AgaveExplorer/remoteFileOps/remotefiletree.cpp \
    ../AgaveExplorer/remoteFileOps/remotejoblister.cpp \
    ../AgaveExplorer/utilWindows/agavesetupdriver.cpp \
    ../AgaveExplorer/utilWindows/authform.cpp \
    ../AgaveExplorer/utilWindows/copyrightdialog.cpp \
    ../AgaveExplorer/utilWindows/deleteconfirm.cpp \
    ../AgaveExplorer/utilWindows/errorpopup.cpp \
    ../AgaveExplorer/utilWindows/quickinfopopup.cpp \
    ../AgaveExplorer/utilWindows/singlelinedialog.cpp \
    ../AgaveExplorer/SimCenterCommon/FooterWidget.cpp \
    ../AgaveExplorer/SimCenterCommon/HeaderWidget.cpp

HEADERS  += \
    vwtinterfacedriver.h \
    ../AgaveClientInterface/agaveInterfaces/agavehandler.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskguide.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskreply.h \
    ../AgaveClientInterface/remotedatainterface.h \
    ../AgaveClientInterface/filemetadata.h \
    ../AgaveClientInterface/remotejobdata.h \
    visualUtils/cfdglcanvas.h \
    visualUtils/cfdtoken.h \
    visualUtils/decompresswrapper.h \
    mainWindow/cwe_mainwindow.h \
    cwe_guiWidgets/cwe_create_simulation.h \
    cwe_guiWidgets/cwe_defines.h \
    cwe_guiWidgets/cwe_file_manager.h \
    cwe_guiWidgets/cwe_help.h \
    cwe_guiWidgets/cwe_landing.h \
    cwe_guiWidgets/cwe_manage_simulation.h \
    cwe_guiWidgets/cwe_simulation_details.h \
    cwe_guiWidgets/cwe_task_list.h \
    cwe_guiWidgets/sidebar.h \
    analysistypes/baseanalysistype.h \
    ../AgaveExplorer/remoteFileOps/easyboollock.h \
    ../AgaveExplorer/remoteFileOps/fileoperator.h \
    ../AgaveExplorer/remoteFileOps/filetreenode.h \
    ../AgaveExplorer/remoteFileOps/joboperator.h \
    ../AgaveExplorer/remoteFileOps/remotefiletree.h \
    ../AgaveExplorer/remoteFileOps/remotejoblister.h \
    ../AgaveExplorer/utilWindows/agavesetupdriver.h \
    ../AgaveExplorer/utilWindows/authform.h \
    ../AgaveExplorer/utilWindows/copyrightdialog.h \
    ../AgaveExplorer/utilWindows/deleteconfirm.h \
    ../AgaveExplorer/utilWindows/errorpopup.h \
    ../AgaveExplorer/utilWindows/quickinfopopup.h \
    ../AgaveExplorer/utilWindows/singlelinedialog.h \
    ../AgaveExplorer/SimCenterCommon/FooterWidget.h \
    ../AgaveExplorer/SimCenterCommon/HeaderWidget.h

FORMS    += \
    mainWindow/cwe_mainwindow.ui \
    cwe_guiWidgets/cwe_create_simulation.ui \
    cwe_guiWidgets/cwe_file_manager.ui \
    cwe_guiWidgets/cwe_help.ui \
    cwe_guiWidgets/cwe_landing.ui \
    cwe_guiWidgets/cwe_manage_simulation.ui \
    cwe_guiWidgets/cwe_simulation_details.ui \
    cwe_guiWidgets/cwe_task_list.ui \
    cwe_guiWidgets/sidebar.ui \
    ../AgaveExplorer/utilWindows/authform.ui \
    ../AgaveExplorer/utilWindows/copyrightdialog.ui \
    ../AgaveExplorer/utilWindows/deleteconfirm.ui \
    ../AgaveExplorer/utilWindows/errorpopup.ui \
    ../AgaveExplorer/utilWindows/quickinfopopup.ui \
    ../AgaveExplorer/utilWindows/singlelinedialog.ui

RESOURCES += \
    cwe_resources.qrc \
    ../AgaveExplorer/SimCenterCommon/commonResources.qrc
