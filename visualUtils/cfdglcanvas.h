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

#include <QMatrix4x4>

#include <math.h>

enum class CFDDisplayState
{
    TEST_BOX,
    MESH,
    FIELD
};

class CFDglCanvas : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    CFDglCanvas(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~CFDglCanvas();

    bool loadMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile);
    bool loadFieldData(QByteArray * rawDataFile, QString valueType);
    bool haveMeshData();
    QString getDisplayError();

    void setDisplayState(CFDDisplayState newState);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    CFDDisplayState myState = CFDDisplayState::TEST_BOX;
    int myWidth;
    int myHeight;

    //Note: this should probably be static const
    double PRECISION = 0.000000001;

    void recomputeProjectionMat(int w, int h);
    void clearMeshData();

    bool isAllZ0(QList<int> aFace);

    QString currentDisplayError;

    QMatrix4x4 projectionMat;

    //Current Mesh Data:
    bool haveValidMeshData = false;
    QList<QList<double>> pointList;
    QList<QList<int>> faceList;
    QList<int> ownerList;

    QList<double> dataList;

    QRectF displayBounds;
    double lowDataVal;
    double highDataVal;
    double dataSpan;

    //QOpenGLVertexArrayObject myVertexArray;
    //QOpenGLBuffer myBuffer;
    /*
    QOpenGLShaderProgram * myShaderProgram = NULL;
    QOpenGLShader * myShader = NULL;
    QOpenGLTexture * myTexture = NULL;
    */
};

#endif // CFDGLCANVAS_H
