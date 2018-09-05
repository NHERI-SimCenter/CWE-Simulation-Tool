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
#include <QDir>
#include <QString>

#include <QDebug>

#include <QtGlobal>

CWE_help::CWE_help(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_help)
{
    ui->setupUi(this);
    ui->helpBrowser->setSource(QUrl("qrc:///help/index.html"));
    ui->helpBrowser->setOpenExternalLinks(true);
}

CWE_help::~CWE_help()
{
    delete ui;
}

void CWE_help::setPageSource(const QUrl &url)
{
    ui->helpBrowser->setSource(url);
}

void CWE_help::on_searchText_editingFinished()
{
    QString searchString = ui->searchText->text();

    qDebug() << "QString: " << searchString;
    qDebug() << "Utf8:    " << searchString.toUtf8();

    //
    // get a list of available html files
    //
    QDir helpDir(":/help");
    QStringList filters;
    filters << "*.html" << "*.HTML" << "*.htm" << "*.HTM";
    QStringList fileList =  helpDir.entryList(filters, QDir::Files);

    //
    // create results document as a string
    //
    QString searchResults;

    searchResults += "<!DOCTYPE html>\
                        <html>\
                        <head>\
                            <style>\
                                xmp  {\
                                    font-family: Consolas, Menlo, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace, serif;\
                                    margin-bottom: 10px;\
                                    overflow: auto;\
                                    width: auto;\
                                    padding: 5px;\
                                    padding-bottom: 20px!ie7;\
                                    max-height: 600px;\
                                    background-color: #cccccc;\
                                      }\
                            </style>\
                        </head>\
                        <body>";

    searchResults += "<h1>Search results for: " + searchString + "</h1>";
    searchResults += "<ol>";

    QByteArray searchPattern = searchString.toLower().toUtf8();

    //
    // search file by file for search string
    //
    foreach (QString fileName, fileList)
    {
        // open help file
        QFile theFile(helpDir.absoluteFilePath(fileName));

        if ( theFile.open(QIODevice::ReadOnly) )
        {
            QByteArray theContents = theFile.readAll().toLower();

            // look for search string in file contents
            if (theContents.contains(searchPattern) )
            {
                int idx = 0;

                while (theContents.indexOf(searchPattern, idx) > 0)
                {
                    idx = theContents.indexOf(searchPattern, idx);
                    int start = idx - 20;
                    if (start < 0) {start = 0;}
                    int length = searchString.length() + 20 + 20;

                    QString textSample = theContents.mid(start, length);

                    // add entry to search result file
                    searchResults += "<li>" ;
                    searchResults += "<a href=\"" + helpDir.relativeFilePath(fileName) + "\">" + helpDir.relativeFilePath(fileName) + "</a>\n";
                    searchResults += "<blockquote><xmp> [...] ";
                    searchResults += textSample.replace("&","&amp;").replace("<","&lt;").replace(">","&gt;");
                    searchResults += " [...] </xmp></blockquote>\n";
                    searchResults += "</li>";

                    idx += searchString.length();
                }
            }

            theFile.close();
        }
    }

    searchResults += "</ol>";

    searchResults += "</body></html>";

    //
    // display search results
    //
    ui->helpBrowser->setHtml(searchResults);

}
