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
    ../AgaveClientInterface/agaveInterfaces/agavelongrunning.cpp \
    fileWindow/fileoperator.cpp \
    fileWindow/remotefilewindow.cpp \
    taskPanelWindow/cfdpanel.cpp \
    taskPanelWindow/debugagaveapppanel.cpp \
    taskPanelWindow/panelwindow.cpp \
    taskPanelWindow/placeholderpanel.cpp \
    taskPanelWindow/taskpanelentry.cpp \
    utilWindows/authform.cpp \
    utilWindows/copyrightdialog.cpp \
    utilWindows/deleteconfirm.cpp \
    utilWindows/errorpopup.cpp \
    utilWindows/quickinfopopup.cpp \
    utilWindows/singlelinedialog.cpp \
    fileWindow/filetreenode.cpp \
    ../AgaveClientInterface/filemetadata.cpp \
    fileWindow/joboperator.cpp \
    taskPanelWindow/simplenamevalpanel.cpp \
    visualUtils/cfdglcanvas.cpp \
    visualUtils/cfdtoken.cpp \
    visualUtils/decompresswrapper.cpp \
    taskPanelWindow/visualpanel.cpp \
    widgets/sidebar.cpp \
    widgets/cwe_landing.cpp \
    widgets/cwe_file_manager.cpp \
    widgets/cwe_help.cpp \
    widgets/cwe_create_simulation.cpp \
    widgets/cwe_manage_simulation.cpp \
    widgets/cwe_simulation_details.cpp \
    widgets/cwe_task_list.cpp \
    mainWindow/cwe_mainwindow.cpp

HEADERS  += \
    vwtinterfacedriver.h \
    ../AgaveClientInterface/agaveInterfaces/agavehandler.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskguide.h \
    ../AgaveClientInterface/agaveInterfaces/agavetaskreply.h \
    ../AgaveClientInterface/remotedatainterface.h \
    ../AgaveClientInterface/agaveInterfaces/agavelongrunning.h \
    fileWindow/fileoperator.h \
    fileWindow/remotefilewindow.h \
    taskPanelWindow/cfdpanel.h \
    taskPanelWindow/debugagaveapppanel.h \
    taskPanelWindow/panelwindow.h \
    taskPanelWindow/placeholderpanel.h \
    taskPanelWindow/taskpanelentry.h \
    utilWindows/authform.h \
    utilWindows/copyrightdialog.h \
    utilWindows/deleteconfirm.h \
    utilWindows/errorpopup.h \
    utilWindows/quickinfopopup.h \
    utilWindows/singlelinedialog.h \
    fileWindow/filetreenode.h \
    ../AgaveClientInterface/filemetadata.h \
    fileWindow/joboperator.h \
    taskPanelWindow/simplenamevalpanel.h \
    visualUtils/cfdglcanvas.h \
    visualUtils/cfdtoken.h \
    visualUtils/decompresswrapper.h \
    taskPanelWindow/visualpanel.h \
    widgets/sidebar.h \
    widgets/cwe_landing.h \
    widgets/cwe_file_manager.h \
    widgets/cwe_help.h \
    widgets/cwe_create_simulation.h \
    widgets/cwe_manage_simulation.h \
    widgets/cwe_simulation_details.h \
    widgets/cwe_task_list.h \
    mainWindow/cwe_mainwindow.h \
    widgets/cwe_defines.h

FORMS    += \
    fileWindow/remotefilewindow.ui \
    taskPanelWindow/panelwindow.ui \
    utilWindows/authform.ui \
    utilWindows/copyrightdialog.ui \
    utilWindows/deleteconfirm.ui \
    utilWindows/errorpopup.ui \
    utilWindows/quickinfopopup.ui \
    utilWindows/singlelinedialog.ui \
    widgets/sidebar.ui \
    widgets/cwe_landing.ui \
    widgets/cwe_file_manager.ui \
    widgets/cwe_help.ui \
    widgets/cwe_create_simulation.ui \
    widgets/cwe_manage_simulation.ui \
    widgets/cwe_simulation_details.ui \
    widgets/cwe_task_list.ui \
    mainWindow/cwe_mainwindow.ui

DISTFILES +=

RESOURCES += \
    cwe_resources.qrc
