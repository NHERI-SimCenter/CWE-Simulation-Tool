#include "resultfield2dwindow.h"

#include "../cfdglcanvas.h"

ResultField2dWindow::ResultField2dWindow(CFDcaseInstance * theCase, QMap<QString, QString> resultDesc, QWidget *parent):
    ResultVisualPopup(theCase, resultDesc, parent) {}

ResultField2dWindow::~ResultField2dWindow(){}

void ResultField2dWindow::initializeView()
{
    //TODO: change to do a field
    QMap<QString, QString> neededFiles;
    neededFiles["points"] = "/constant/polyMesh/points.gz";
    neededFiles["faces"] = "/constant/polyMesh/faces.gz";
    neededFiles["owner"] = "/constant/polyMesh/owner.gz";

    performStandardInit(neededFiles);
}

void ResultField2dWindow::allFilesLoaded()
{
    //TODO: change to do a field
    QObject::disconnect(this);
    QMap<QString, QByteArray *> fileBuffers = getFileBuffers();

    CFDglCanvas * myCanvas;
    changeDisplayFrameTenant(myCanvas = new CFDglCanvas());

    myCanvas->loadMeshData(fileBuffers["points"], fileBuffers["faces"], fileBuffers["owner"]);

    if (!myCanvas->haveMeshData())
    {
        changeDisplayFrameTenant(new QLabel("Error: Data for 2D mesh result is unreadable. Please reset and try again."));
        return;
    }

    myCanvas->setDisplayState(CFDDisplayState::MESH);
}
