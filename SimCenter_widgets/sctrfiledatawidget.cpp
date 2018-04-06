/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
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

#include "sctrfiledatawidget.h"

#include "../AgaveExplorer/remoteModelViews/remotefiletree.h"
#include "../AgaveExplorer/remoteFileOps/filetreenode.h"
#include "../AgaveClientInterface/filemetadata.h"
#include "cwe_interfacedriver.h"

SCtrFileDataWidget::SCtrFileDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

SCtrFileDataWidget::SCtrFileDataWidget(CWE_InterfaceDriver *theDriver, QWidget *parent):
    SCtrMasterDataWidget(parent)
{
    myDriver = theDriver;
}

void SCtrFileDataWidget::initUI()
{
    if (label_varName == NULL) {
        label_varName = new QLabel(this);
    }
    if (selectedFile == NULL)
    {
        selectedFile = new QLabel(this);
    }
    explainText = new QLabel("\nIn order to run a simulation, a geometry file must be uploaded.\nClick on the files tab to go to the upload/download screen.\nCWE can use \"Alias Mesh\" .obj files exported from FreeCAD, as well as our own JSON geometry format.\n\nSelected File:");
    explainText->setMaximumWidth(400);
    explainText->setWordWrap(true);
    QBoxLayout *fullLayout = new QHBoxLayout();
    QBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addWidget(label_varName);
    leftLayout->addWidget(explainText);
    leftLayout->addWidget(selectedFile);
    fullLayout->addItem(leftLayout);
    this->setLayout(fullLayout);
}

void SCtrFileDataWidget::setData(QJsonObject &obj)
{
    // set up the UI for the widget
    this->initUI();

    m_obj = obj;

    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    myFileTree = new RemoteFileTree(this);
    myFileTree->setEditTriggers(QTreeView::NoEditTriggers);
    layout->insertWidget(1, myFileTree, 4);

    if (label_varName != NULL) {
        label_varName->setText(obj.value(QString("displayname")).toString());
    }

    if (selectedFile != NULL) {
        selectedFile->setText(obj.value(QString("default")).toString());
    }

    QObject::connect(myFileTree, SIGNAL(newFileSelected(FileNodeRef)),
                     this, SLOT(newFileSelected(FileNodeRef)));
}

bool SCtrFileDataWidget::toBool()
{
    QString text = selectedFile->text();
    return !text.isEmpty();
}

QString SCtrFileDataWidget::toString()
{
    return selectedFile->text();
}

void SCtrFileDataWidget::updateValue(QString s)
{
    /* update the value */
    selectedFile->setText(s);
    myFileTree->clearSelection();
}

void SCtrFileDataWidget::newFileSelected(FileNodeRef newFile)
{
    if (newFile.isNil())
    {
        selectedFile->setText("");
    }
    selectedFile->setText(newFile.getFullPath());
}
