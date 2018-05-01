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

#include <QApplication>
#include <QFile>
#include <QSslSocket>
#include <QtGlobal>

#include "../AgaveClientInterface/remotedatainterface.h"

#include "cwe_interfacedriver.h"
#include "cwe_globals.h"

void emptyMessageHandler(QtMsgType, const QMessageLogContext &, const QString &){}

QString openStyleFiles()
{
    QString ret;
    QFile mainStyleFile(":/styleSheets/cweStyle.qss");

#ifdef Q_OS_WIN
    QFile appendedStyle(":/styleSheets/cweWin.qss");
#endif

#ifdef Q_OS_MACOS
    QFile appendedStyle(":/styleSheets/cweMac.qss");
#endif

#ifdef Q_OS_LINUX
    QFile appendedStyle(":/styleSheets/cweLinux.qss");
#endif

    if (!mainStyleFile.open(QFile::ReadOnly))
    {
        cwe_globals::displayFatalPopup("Unable to open main style file. Install may be corrupted.");
        return ret;
    }

    if (!appendedStyle.open(QFile::ReadOnly))
    {
        cwe_globals::displayFatalPopup("Unable to open platform style file. Install may be corrupted.");
        return ret;
    }

    ret = ret.append(mainStyleFile.readAll());
    ret = ret.append(appendedStyle.readAll());

    mainStyleFile.close();
    appendedStyle.close();

    return ret;
}

int main(int argc, char *argv[])
{
    QApplication mainRunLoop(argc, argv);

    bool debugLoggingEnabled = false;
    bool runOffline = false;
    bool logRawOutput = false;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i],"enableDebugLogging") == 0)
        {
            debugLoggingEnabled = true;
        }
        if (strcmp(argv[i],"offlineMode") == 0)
        {
            runOffline = true;
        }
        if (strcmp(argv[i],"logRawOutput") == 0)
        {
            logRawOutput = true;
        }
    }

    if (runOffline)
    {
        qDebug("NOTE: Running CWE client offline.");
    }

    if (debugLoggingEnabled)
    {
        qDebug("NOTE: Debugging text output is enabled.");
    }
    else
    {
        qInstallMessageHandler(emptyMessageHandler);
    }

    CWE_InterfaceDriver programDriver(nullptr, debugLoggingEnabled);
    if (QSslSocket::supportsSsl() == false)
    {
        cwe_globals::displayFatalPopup("SSL support was not detected on this computer.\nPlease insure that some version of SSL is installed,\n such as by installing OpenSSL.");
    }

    mainRunLoop.setStyleSheet(openStyleFiles());

    mainRunLoop.setQuitOnLastWindowClosed(false);
    //Note: Window closeing must link to the shutdown sequence, otherwise the app will not close
    //Note: Might consider a better way of implementing this.

    if (runOffline)
    {
        programDriver.startOffline();
    }
    else
    {
        programDriver.startup();
    }

    if (debugLoggingEnabled && logRawOutput)
    {
        qDebug("NOTE: Debugging text including raw remote output.");
        programDriver.getDataConnection()->setRawDebugOutput(true);
    }

    return mainRunLoop.exec();
}
