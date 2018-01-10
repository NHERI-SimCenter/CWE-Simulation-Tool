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

CWE_Result_Popup::CWE_Result_Popup(QString caseName, QString caseType, QMap<QString, QString> theResult, VWTinterfaceDriver * theDriver, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Result_Popup)
{
    ui->setupUi(this);
    myDriver = theDriver;
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

    if (resultType == "GLmesh")
    {
        targetFolder = currentCase->getCaseFolder().append("/").append(theResult.value("stage")).append("/constant/polyMesh/");
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

    if (folderNode->childIsUnloaded())
    {
        myDriver->getFileHandler()->lsClosestNode(targetFolder);
        return;
    }

    if (resultType == "GLmesh")
    {
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

        myCanvas = new CFDglCanvas();
        myCanvas->loadMeshData(realPointsBuffer, realFacesBuffer, realOwnerBuffer);
        myCanvas->setDisplayState(CFDDisplayState::MESH);
        myCanvas->show();

        loadingLabel->deleteLater();
        loadingLabel = NULL;
        ui->displayFrame->layout()->addWidget(myCanvas);

        if (realPointsBuffer != pointsBuffer) delete realPointsBuffer;
        if (realFacesBuffer != facesBuffer) delete realFacesBuffer;
        if (realOwnerBuffer != ownerBuffer) delete realOwnerBuffer;
        delete pointsBuffer;
        delete facesBuffer;
        delete ownerBuffer;

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
