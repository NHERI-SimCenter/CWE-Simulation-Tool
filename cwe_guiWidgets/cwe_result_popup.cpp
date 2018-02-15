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

#include "cwe_result_popup.h"
#include "ui_cwe_result_popup.h"

#include "../AgaveClientInterface/filemetadata.h"

#include "../AgaveExplorer/remoteFileOps/fileoperator.h"
#include "../AgaveExplorer/remoteFileOps/filetreenode.h"

#include "CFDanalysis/CFDcaseInstance.h"
#include "visualUtils/cfdglcanvas.h"
#include "visualUtils/decompresswrapper.h"
#include "vwtinterfacedriver.h"
#include "cwe_globals.h"

CWE_Result_Popup::CWE_Result_Popup(QString caseName, QString caseType, QMap<QString, QString> theResult, VWTinterfaceDriver * theDriver, bool downloadResult, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Result_Popup)
{
    ui->setupUi(this);
    myDriver = theDriver;
    download = downloadResult;
    CFDcaseInstance * currentCase = myDriver->getCurrentCase();
    if (currentCase == NULL)
    {
        this->deleteLater();
        return;
    }

    ui->label_theName->setText(caseName);
    ui->label_theType->setText(caseType);
    ui->label_result->setText(theResult.value("name"));

    resultType = theResult.value("type");
    myResult = theResult;

    if ((resultType == "GLmesh") || (resultType == "GLdata"))
    {
        targetFolder = currentCase->getCaseFolder().append("/").append(theResult.value("stage")).append("/constant/polyMesh/");
        if (download)
        {
            this->deleteLater();
            return;
        }
        if (resultType == "GLdata")
        {
            targetFile = currentCase->getCaseFolder().append("/").append(theResult.value("stage")).append("/");
            QString dataType = theResult.value("values");
            if ((dataType != "magnitude") &&
                    (dataType != "scalar"))
            {
                cwe_globals::displayPopup("Error: Unknown result type for display.");
                this->deleteLater();
                return;
            }
        }
    }
    else if (resultType == "text")
    {
        targetFile = currentCase->getCaseFolder().append("/").append(theResult.value("stage")).append("/").append(theResult.value("file"));
    }
    else
    {
        cwe_globals::displayPopup("Error: Unknown result type for display.");
        this->deleteLater();
        return;
    }
    loadingLabel = new QLabel("Loading remote data. Please Wait.");
    QHBoxLayout * resultFrameLayout = new QHBoxLayout();
    resultFrameLayout->addWidget(loadingLabel);
    ui->displayFrame->setLayout(resultFrameLayout);

    QObject::connect(myDriver->getFileHandler(),SIGNAL(fileSystemChange()),
                     this, SLOT(newFileInfo()));

    newFileInfo();
}

CWE_Result_Popup::~CWE_Result_Popup()
{
    delete ui;
    if (myCanvas != NULL)
    {
        myCanvas->deleteLater();
    }
    if (loadingLabel != NULL)
    {
        loadingLabel->deleteLater();
    }
}

void CWE_Result_Popup::closeButtonClicked()
{
    this->deleteLater();
    QObject::disconnect(myDriver->getFileHandler(),SIGNAL(fileSystemChange()),
                     this, SLOT(newFileInfo()));
}

void CWE_Result_Popup::newFileInfo()
{
    if ((resultType == "GLmesh") || (resultType == "GLdata"))
    {
        FileTreeNode * folderNode = myDriver->getFileHandler()->getNodeFromName(targetFolder);

        if (folderNode == NULL)
        {
            folderNode = myDriver->getFileHandler()->getClosestNodeFromName(targetFolder);

            if (folderNode == NULL)
            {
                cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                this->deleteLater();
                return;
            }

            myDriver->getFileHandler()->lsClosestNode(targetFolder);
            return;
        }

        if (folderNode->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED)
        {
            myDriver->getFileHandler()->lsClosestNode(targetFolder);
            return;
        }

        FileTreeNode * pointsFile = folderNode->getChildNodeWithName("points");
        FileTreeNode * facesFile = folderNode->getChildNodeWithName("faces");
        FileTreeNode * ownerFile = folderNode->getChildNodeWithName("owner");
        if (pointsFile == NULL) pointsFile = folderNode->getChildNodeWithName("points.gz");
        if (facesFile == NULL) facesFile = folderNode->getChildNodeWithName("faces.gz");
        if (ownerFile == NULL) ownerFile = folderNode->getChildNodeWithName("owner.gz");

        if ((pointsFile == NULL) || (facesFile == NULL) || (ownerFile == NULL))
        {
            cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
            this->deleteLater();
            return;
        }

        if ((myCanvas == NULL) || (!myCanvas->haveMeshData()))
        {
            QByteArray * pointsBuffer = pointsFile->getFileBuffer();
            QByteArray * facesBuffer = facesFile->getFileBuffer();
            QByteArray * ownerBuffer = ownerFile->getFileBuffer();

            if (pointsBuffer == NULL)
            {
                myDriver->getFileHandler()->sendDownloadBuffReq(pointsFile);
            }
            if (facesBuffer == NULL)
            {
                myDriver->getFileHandler()->sendDownloadBuffReq(facesFile);
            }
            if (ownerBuffer == NULL)
            {
                myDriver->getFileHandler()->sendDownloadBuffReq(ownerFile);
            }
            if ((pointsBuffer == NULL) || (facesBuffer == NULL) || (ownerBuffer == NULL))
            {
                return;
            }

            QByteArray * realPointsBuffer = pointsBuffer;
            QByteArray * realFacesBuffer = facesBuffer;
            QByteArray * realOwnerBuffer = ownerBuffer;

            if (pointsFile->getFileData().getFileName().endsWith(".gz"))
            {
                DeCompressWrapper realPoints(pointsBuffer);
                realPointsBuffer = realPoints.getDecompressedFile();
            }
            if (pointsFile->getFileData().getFileName().endsWith(".gz"))
            {
                DeCompressWrapper realFaces(facesBuffer);
                realFacesBuffer = realFaces.getDecompressedFile();
            }
            if (pointsFile->getFileData().getFileName().endsWith(".gz"))
            {
                DeCompressWrapper realOwner(ownerBuffer);
                realOwnerBuffer = realOwner.getDecompressedFile();
            }

            if (myCanvas != NULL) myCanvas->deleteLater();
            myCanvas = new CFDglCanvas();
            myCanvas->loadMeshData(realPointsBuffer, realFacesBuffer, realOwnerBuffer);
            if (realPointsBuffer != pointsBuffer) delete realPointsBuffer;
            if (realFacesBuffer != facesBuffer) delete realFacesBuffer;
            if (realOwnerBuffer != ownerBuffer) delete realOwnerBuffer;

            if (!myCanvas->haveMeshData())
            {
                cwe_globals::displayPopup("Error: Data for result display is unreadable. Please reset and try again.");
                this->deleteLater();
                return;
            }
        }

        if (resultType == "GLmesh")
        {
            myCanvas->setDisplayState(CFDDisplayState::MESH);
            myCanvas->show();

            loadingLabel->deleteLater();
            loadingLabel = NULL;
            ui->displayFrame->layout()->addWidget(myCanvas);

            QObject::disconnect(myDriver->getFileHandler(),SIGNAL(fileSystemChange()),
                             this, SLOT(newFileInfo()));
        }
        else if (resultType == "GLdata")
        {
            FileTreeNode * baseFolderNode = myDriver->getFileHandler()->getNodeFromName(targetFile);

            if (baseFolderNode == NULL)
            {
                baseFolderNode = myDriver->getFileHandler()->getClosestNodeFromName(targetFile);
                if (baseFolderNode == NULL)
                {
                    cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                    this->deleteLater();
                    return;
                }

                myDriver->getFileHandler()->lsClosestNode(targetFile);
                return;
            }

            if (baseFolderNode->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED)
            {
                myDriver->getFileHandler()->lsClosestNode(targetFile);
                return;
            }

            double biggestNum = -1;
            FileTreeNode * targetChild = NULL;
            QList<FileTreeNode *> childList = baseFolderNode->getChildList();
            for (auto itr = childList.cbegin(); itr != childList.cend(); itr++)
            {
                if ((*itr)->getFileData().getFileType() != FileType::DIR)
                {
                    continue;
                }
                QString childName = (*itr)->getFileData().getFileName();
                if (childName == "0") continue;

                bool isNum = false;
                double childVal = childName.toDouble(&isNum);
                if (isNum)
                {
                    if (biggestNum < childVal)
                    {
                        biggestNum = childVal;
                        targetChild = (*itr);
                    }
                }
            }

            if (targetChild == NULL)
            {
                cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                this->deleteLater();
                return;
            }

            if (targetChild->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED)
            {
                myDriver->getFileHandler()->lsClosestNode(targetChild->getFileData().getFullPath());
                return;
            }

            if (myResult.value("file").isEmpty())
            {
                cwe_globals::displayPopup("Error: Invalid template configuration.");
                this->deleteLater();
                return;
            }

            bool zipped = false;
            FileTreeNode * finalDataFile = targetChild->getChildNodeWithName(myResult.value("file"));
            QByteArray * dataBuffer = NULL;;
            if (finalDataFile == NULL)
            {
                zipped = true;
                QString zipFileName = myResult.value("file");
                zipFileName = zipFileName.append(".gz");
                finalDataFile = targetChild->getChildNodeWithName(zipFileName);
                if (finalDataFile == NULL)
                {
                    cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                    this->deleteLater();
                    return;
                }
                QByteArray * rawBuffer = finalDataFile->getFileBuffer();
                if (rawBuffer == NULL)
                {
                    myDriver->getFileHandler()->sendDownloadBuffReq(finalDataFile);
                    return;
                }
                DeCompressWrapper dataExtractor(rawBuffer);
                dataBuffer = dataExtractor.getDecompressedFile();
            }
            else
            {
                QByteArray * rawBuffer = finalDataFile->getFileBuffer();
                if (rawBuffer == NULL)
                {
                    myDriver->getFileHandler()->sendDownloadBuffReq(finalDataFile);
                    return;
                }
                dataBuffer = rawBuffer;
            }

            if (dataBuffer == NULL)
            {
                cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                this->deleteLater();
                return;
            }

            myCanvas->loadFieldData(dataBuffer, myResult.value("values"));
            if (zipped)
            {
                delete dataBuffer;
            }
            myCanvas->setDisplayState(CFDDisplayState::FIELD);
            myCanvas->show();

            loadingLabel->deleteLater();
            loadingLabel = NULL;
            ui->displayFrame->layout()->addWidget(myCanvas);

            QObject::disconnect(myDriver->getFileHandler(),SIGNAL(fileSystemChange()),
                             this, SLOT(newFileInfo()));
        }
        else
        {
            cwe_globals::displayPopup("Error: Data for result display is unreadable. Please reset and try again.");
            this->deleteLater();
            return;
        }
    }
    else if (resultType == "text")
    {
        FileTreeNode * fileNode = myDriver->getFileHandler()->getNodeFromName(targetFile);

        if (fileNode == NULL)
        {
            fileNode = myDriver->getFileHandler()->getClosestNodeFromName(targetFile);

            if ((fileNode == NULL) || (fileNode->getNodeState() != NodeState::FOLDER_CONTENTS_LOADED)) //TODO: This check needs to go everywhere on file re-write
            {
                cwe_globals::displayPopup("Error: Data for result display is unavailable. Please reset and try again.");
                this->deleteLater();
                return;
            }

            myDriver->getFileHandler()->lsClosestNode(targetFile);
            return;
        }

        QByteArray * fileText = fileNode->getFileBuffer();

        if (fileText == NULL)
        {
            myDriver->getFileHandler()->sendDownloadBuffReq(fileNode);
        }

        if (download)
        {
            this->deleteLater();

            QString fileName = QFileDialog::getSaveFileName(this, "Select Destination File:");
            if (fileName.isEmpty())
            {
                return;
            }

            QFile destFile(fileName);
            if (!destFile.open(QFile::WriteOnly))
            {
                cwe_globals::displayPopup("Error: Cannot open local file.");
                return;
            }
            destFile.write(*fileText);
            destFile.close();
            cwe_globals::displayPopup("Result File Downloaded.");
        }
        else
        {
            textBox = new QPlainTextEdit(this);
            textBox->setReadOnly(true);
            textBox->setPlainText(QString::fromStdString(fileText->toStdString()));

            loadingLabel->deleteLater();
            loadingLabel = NULL;
            ui->displayFrame->layout()->addWidget(myCanvas);
        }

        QObject::disconnect(myDriver->getFileHandler(),SIGNAL(fileSystemChange()),
                         this, SLOT(newFileInfo()));
    }
    else
    {
        cwe_globals::displayPopup("Error: Unknown result type for display.");
        this->deleteLater();
        return;
    }
}
