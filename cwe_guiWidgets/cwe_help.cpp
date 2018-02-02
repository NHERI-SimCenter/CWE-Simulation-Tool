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

    this->setOverview();
}

CWE_help::~CWE_help()
{
    delete ui;
}

void CWE_help::setOverview()
{
    QFile overview(":/help/overview.html");
    if (!overview.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    QByteArray theText = overview.readAll();
    overview.close();

    ui->textBrowser_overview->setHtml(theText);
    ui->textBrowser_overview->setOpenLinks(false);
    ui->helpTabs->setTabsClosable(true);

    QObject::connect(ui->textBrowser_overview, SIGNAL(anchorClicked(const QUrl &)),
                     this, SLOT(on_Overview_anchorClicked(const QUrl &)));
}

void CWE_help::on_Overview_anchorClicked(const QUrl &link)
{
    QFile helpText(link.toLocalFile());
    if (!helpText.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    QByteArray theText = helpText.readAll();
    helpText.close();

    QTextBrowser *browser = new QTextBrowser(this);
    browser->setOpenLinks(true);
    browser->setHtml(theText);

    QString theLabel = QFileInfo(link.fileName()).completeBaseName();
    int tabIdx = ui->helpTabs->addTab(browser, theLabel);
    ui->helpTabs->setCurrentIndex(tabIdx);
}
