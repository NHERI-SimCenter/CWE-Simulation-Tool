#include "resultfield2dwindow.h"

#include "../cfdglcanvas.h"

ResultField2dWindow::ResultField2dWindow(CFDcaseInstance * theCase, QMap<QString, QString> resultDesc, QWidget *parent):
    ResultVisualPopup(theCase, resultDesc, parent) {}

ResultField2dWindow::~ResultField2dWindow(){}

void ResultField2dWindow::initializeView()
{
    QMap<QString, QString> neededFiles;
    neededFiles["points"] = "/constant/polyMesh/points.gz";
    neededFiles["faces"] = "/constant/polyMesh/faces.gz";
    neededFiles["owner"] = "/constant/polyMesh/owner.gz";

    QString fieldName = getResultObj()["file"];
    QString fieldFile = "[final]/";
    fieldFile.append(fieldName).append(".gz");
    neededFiles["data"] = fieldFile;

    performStandardInit(neededFiles);
}

void ResultField2dWindow::allFilesLoaded()
{
    QObject::disconnect(this);
    QMap<QString, QByteArray *> fileBuffers = getFileBuffers();

    CFDglCanvas * myCanvas;
    changeDisplayFrameTenant(myCanvas = new CFDglCanvas());

    myCanvas->loadMeshData(fileBuffers["points"], fileBuffers["faces"], fileBuffers["owner"]);

    if (!myCanvas->haveMeshData())
    {
        changeDisplayFrameTenant(new QLabel("Error: Data for 2D mesh is unreadable. Please reset and try again."));
        return;
    }

    myCanvas->loadFieldData(fileBuffers["data"], getResultObj()["values"]);
    myCanvas->setDisplayState(CFDDisplayState::FIELD);
}
