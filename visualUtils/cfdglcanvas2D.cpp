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

#include "cfdglcanvas2D.h"

CFDglCanvas2D::CFDglCanvas2D(QWidget *parent, Qt::WindowFlags f) : CFDglCanvas(parent,f) {}

CFDglCanvas2D::~CFDglCanvas2D() {}

bool CFDglCanvas2D::loadMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile)
{
    return loadRawMeshData(rawPointFile, rawFaceFile, rawOwnerFile);
}

void CFDglCanvas2D::mousePressEvent(QMouseEvent *event)
{
    lastXmousePos = event->x();
    lastYmousePos = event->y();
    if ((event->buttons() & (Qt::LeftButton | Qt::RightButton)) != 0)
    {
        setMouseTracking(true);
    }
}

void CFDglCanvas2D::mouseReleaseEvent(QMouseEvent *event)
{
    lastXmousePos = event->x();
    lastYmousePos = event->y();
    if ((event->buttons() & (Qt::LeftButton | Qt::RightButton)) == 0)
    {
        setMouseTracking(false);
    }
}

void CFDglCanvas2D::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        int deltaX = event->x() - lastXmousePos;
        int deltaY = event->y() - lastYmousePos;

        panXdist += deltaX * distByPixelX;
        panYdist -= deltaY * distByPixelY;
        recomputeViewModelMat();
        this->update();
    }

    lastXmousePos = event->x();
    lastYmousePos = event->y();
}

void CFDglCanvas2D::wheelEvent(QWheelEvent *event)
{
    QPoint scrollDegrees = event->angleDelta();
    if (scrollDegrees.isNull()) return;

    zoomTicks += scrollDegrees.y();
    recomputePerspecMat();
    this->update();
}

void CFDglCanvas2D::paintGL()
{
    if (!readyToDisplay) return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(projMat.data());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(viewModelMat.data());

    glClear(GL_COLOR_BUFFER_BIT);

    if (dataList.isEmpty())
    {
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINES);

        for (auto faceItr = faceList.cbegin(); faceItr != faceList.cend(); faceItr++)
        {
            QList<int> aFace = (*faceItr);
            bool allZ0 = isAllZ0(aFace);

            if (allZ0)
            {
                glVertex3f(static_cast<GLfloat>(pointList.at(aFace.last()).at(0)),
                           static_cast<GLfloat>(pointList.at(aFace.last()).at(1)),0.0);
                glVertex3f(static_cast<GLfloat>(pointList.at(aFace.first()).at(0)),
                           static_cast<GLfloat>(pointList.at(aFace.first()).at(1)),0.0);

                for (int ind = 1; ind < aFace.size(); ind++)
                {
                    glVertex3f(static_cast<GLfloat>(pointList.at(aFace.at(ind - 1)).at(0)),
                               static_cast<GLfloat>(pointList.at(aFace.at(ind - 1)).at(1)),0.0);
                    glVertex3f(static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(0)),
                               static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(1)),0.0);
                }
            }
        }
        glEnd();
        return;
    }

    int indexVal = -1;
    for (auto faceItr = faceList.cbegin(); faceItr != faceList.cend(); faceItr++)
    {
        indexVal++;
        QList<int> aFace = (*faceItr);
        bool allZ0 = isAllZ0(aFace);

        if (allZ0)
        {
            double rawData = dataList.at(ownerList.at(indexVal)); //TODO: Probably should check bounds

            double dataVal = (rawData - lowDataVal) / (highDataVal - lowDataVal);

            glBegin(GL_POLYGON);
            double redVal = 1.0;
            double greenVal = 0.0;
            double blueVal = 1.0;

            if (dataVal > 1.0) dataVal = 1.0;
            else if (dataVal < 0.0) dataVal = 0.0;

            if (dataVal > 0.5)
            {
                blueVal = 0.3 + 0.7 * ((1.0 - dataVal) / 0.5);
                greenVal = 0.3 + 0.7 * ((1.0 - dataVal) / 0.5);
            }
            else
            {
                redVal = 0.3 + 0.7 * (dataVal / 0.5);
                greenVal = 0.3 + 0.7 * (dataVal / 0.5);
            }

            glColor3f(static_cast<GLfloat>(redVal),
                      static_cast<GLfloat>(greenVal),
                      static_cast<GLfloat>(blueVal));

            for (int ind = 0; ind < aFace.size(); ind++)
            {
                glVertex3f(static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(0)),
                           static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(1)),0.0);
            }
            glEnd();
        }
    }
}

void CFDglCanvas2D::recomputePerspecMat()
{
    projMat.setToIdentity();
    if (!readyToDisplay) return;

    double objWidth = modelBounds2D.right() - modelBounds2D.left();
    double objHeight = modelBounds2D.top() - modelBounds2D.bottom();
    double wRatio = objWidth / myDisplayWidth;
    double hRatio = objHeight / myDisplayHeight;

    double scaleFactor = 1.0 / qPow(2.0, (static_cast<double>(zoomTicks))/ ZOOMFACTOR2D);

    if (wRatio > hRatio)
    {
        double correctionFactor = wRatio / hRatio;

        projMat.ortho(-static_cast<float>(scaleFactor * objWidth/2),
                      static_cast<float>(scaleFactor * objWidth/2),
                      -static_cast<float>(scaleFactor * correctionFactor * objHeight/2),
                      static_cast<float>(scaleFactor * correctionFactor * objHeight/2),
                      -1.0f,1.0f);

        distByPixelX = scaleFactor * objWidth / myDisplayWidth;
        distByPixelY = scaleFactor * correctionFactor * objHeight / myDisplayHeight;
    }
    else
    {
        double correctionFactor = hRatio / wRatio;

        projMat.ortho(-static_cast<float>(scaleFactor * correctionFactor * objWidth/2),
                      static_cast<float>(scaleFactor * correctionFactor * objWidth/2),
                      -static_cast<float>(scaleFactor * objHeight/2),
                      static_cast<float>(scaleFactor * objHeight/2),
                      -1.0f,1.0f);

        distByPixelX = scaleFactor * correctionFactor * objWidth / myDisplayWidth;
        distByPixelY = scaleFactor * objHeight / myDisplayHeight;
    }
}

void CFDglCanvas2D::recomputeViewModelMat()
{
    viewModelMat.setToIdentity();

    viewModelMat.translate(panXdist, panYdist);
    viewModelMat.translate(static_cast<float>(-modelBounds2D.center().x()),
                           static_cast<float>(-modelBounds2D.center().y()));
}
