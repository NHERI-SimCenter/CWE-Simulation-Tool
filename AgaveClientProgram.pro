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

TARGET = AgaveClientProgram
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    vwtinterfacedriver.cpp \
    taskPanels/placeholderpanel.cpp \
    taskPanels/taskpanelentry.cpp \
    taskPanels/filemainippanel.cpp \
    programWindows/authform.cpp \
    programWindows/errorpopup.cpp \
    programWindows/panelwindow.cpp \
    agaveInterfaces/agavehandler.cpp \
    agaveInterfaces/agavetaskguide.cpp \
    agaveInterfaces/agavetaskreply.cpp \
    programWindows/deleteconfirm.cpp \
    programWindows/singlelinedialog.cpp \
    programWindows/copyrightdialog.cpp \
    programWindows/filetreemodelreader.cpp \
    remotedatainterface.cpp \
    programWindows/quickinfopopup.cpp

HEADERS  += \
    vwtinterfacedriver.h \
    taskPanels/placeholderpanel.h \
    taskPanels/taskpanelentry.h \
    taskPanels/filemainippanel.h \
    programWindows/authform.h \
    programWindows/errorpopup.h \
    programWindows/panelwindow.h \
    agaveInterfaces/agavehandler.h \
    agaveInterfaces/agavetaskguide.h \
    agaveInterfaces/agavetaskreply.h \
    programWindows/deleteconfirm.h \
    programWindows/singlelinedialog.h \
    programWindows/copyrightdialog.h \
    programWindows/filetreemodelreader.h \
    remotedatainterface.h \
    programWindows/quickinfopopup.h

FORMS    += \
    programWindows/authform.ui \
    programWindows/errorpopup.ui \
    programWindows/panelwindow.ui \
    programWindows/deleteconfirm.ui \
    programWindows/singlelinedialog.ui \
    programWindows/copyrightdialog.ui \
    programWindows/quickinfopopup.ui
