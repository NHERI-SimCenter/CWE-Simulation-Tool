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

#ifndef CFDGLCANVAS_H
#define CFDGLCANVAS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>

#include <QMatrix4x4>

#include <QtMath>

class CFDglCanvas : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    CFDglCanvas(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CFDglCanvas();

    virtual bool loadMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile) = 0;
    bool loadFieldData(QByteArray * rawDataFile, QString valueType);

    bool displayAvailData();
    QString getDisplayError();

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);

    bool isAllZ0(QList<int> aFace);
    bool loadRawMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile);
    void clearAllData();

    QList<QList<double>> pointList;
    QList<QList<int>> faceList;
    QList<int> ownerList;
    QList<double> dataList;

    bool readyToDisplay = false;
    QString currentDisplayError;

    QRectF modelBounds2D;
    int myDisplayWidth;
    int myDisplayHeight;
    double lowDataVal;
    double highDataVal;

    constexpr static const double PRECISION = 0.000000001;

private:
    virtual void recomputePerspecMat() = 0;
    virtual void recomputeViewModelMat() = 0;
};

#endif // CFDGLCANVAS_H
