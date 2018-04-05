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

#include "cwe_help.h"
#include "ui_cwe_help.h"

#include <QFile>
#include <QFileInfo>
#include <QUrl>

CWE_help::CWE_help(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_help)
{
    ui->setupUi(this);

    QObject::connect(ui->helpBrowser, SIGNAL(anchorClicked(QUrl)),
                     this, SLOT(browser_anchor_clicked(QUrl)));

    QFile headText(":/help/common_header.html");
    if (!headText.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    headerText = headText.readAll();
    headText.close();

    QFile footText(":/help/common_footer.html");
    if (!footText.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    footerText = footText.readAll();
    footText.close();

    setLocalPage(":/help/index.html");
}

CWE_help::~CWE_help()
{
    delete ui;
}

void CWE_help::setLocalPage(QString pageName)
{
    QFile helpText(pageName);
    if (!helpText.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        if (currentPageName == pageName)
        {
            setLocalPage(":/help/index.html");
        }
        else
        {
            setLocalPage(currentPageName);
        }
        return;
    }

    currentPageName = pageName;
    QByteArray theText;
    theText = theText.append(headerText);
    theText = theText.append(helpText.readAll());
    theText = theText.append(footerText);

    helpText.close();

    ui->helpBrowser->setHtml(theText);
}

void CWE_help::browser_anchor_clicked(const QUrl &link)
{
    if (link.isLocalFile())
    {
        setLocalPage(link.toLocalFile());
        return;
    }

    QDesktopServices::openUrl(link);
    setLocalPage(currentPageName);
}
