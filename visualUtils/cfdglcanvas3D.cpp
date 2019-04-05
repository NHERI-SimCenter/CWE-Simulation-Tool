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

#include "cfdglcanvas3D.h"

CFDglCanvas3D::CFDglCanvas3D(QWidget *parent, Qt::WindowFlags f) : CFDglCanvas(parent,f) {}

CFDglCanvas3D::~CFDglCanvas3D() {}

bool CFDglCanvas3D::loadMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile)
{
    if (!loadRawMeshData(rawPointFile, rawFaceFile, rawOwnerFile)) return false;

    double highz = pointList.at(0).at(2);
    double lowz = pointList.at(0).at(2);

    for (auto itr = pointList.cbegin(); itr != pointList.cend(); itr++)
    {
        double zVal = (*itr).at(2);

        if (zVal > highz) highz = zVal;
        if (zVal < lowz) lowz = zVal;
    }

    centerz = lowz + (highz - lowz)/2.0;

    return true;
}

void CFDglCanvas3D::paintGL()
{
    if (!readyToDisplay) return;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(projMat.data());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(viewModelMat.data());

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);

    for (auto faceItr = faceList.cbegin(); faceItr != faceList.cend(); faceItr++)
    {
        QList<int> aFace = (*faceItr);

        glVertex3f(static_cast<GLfloat>(pointList.at(aFace.last()).at(0)),
                   static_cast<GLfloat>(pointList.at(aFace.last()).at(1)),
                   static_cast<GLfloat>(pointList.at(aFace.last()).at(2)));
        glVertex3f(static_cast<GLfloat>(pointList.at(aFace.first()).at(0)),
                   static_cast<GLfloat>(pointList.at(aFace.first()).at(1)),
                   static_cast<GLfloat>(pointList.at(aFace.first()).at(2)));

        for (int ind = 1; ind < aFace.size(); ind++)
        {
            glVertex3f(static_cast<GLfloat>(pointList.at(aFace.at(ind - 1)).at(0)),
                       static_cast<GLfloat>(pointList.at(aFace.at(ind - 1)).at(1)),
                       static_cast<GLfloat>(pointList.at(aFace.at(ind - 1)).at(2)));
            glVertex3f(static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(0)),
                       static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(1)),
                       static_cast<GLfloat>(pointList.at(aFace.at(ind)).at(2)));
        }
    }
    glEnd();
}

void CFDglCanvas3D::recomputePerspecMat()
{
    projMat.setToIdentity();
    if (!readyToDisplay) return;

    double ywidth = modelBounds2D.top() - modelBounds2D.bottom();

    projMat.perspective(45.0f, myDisplayWidth / float(myDisplayHeight), 0.01f,
                        static_cast<float>(4.0*ywidth));
}

void CFDglCanvas3D::recomputeViewModelMat()
{
    viewModelMat.setToIdentity();
    double ywidth = modelBounds2D.top() - modelBounds2D.bottom();

    viewModelMat.lookAt(QVector3D(static_cast<float>(modelBounds2D.center().x()),
                                 static_cast<float>(modelBounds2D.center().y() - 2.5*ywidth),
                                 static_cast<float>(centerz)),
                       QVector3D(static_cast<float>(modelBounds2D.center().x()),
                                 static_cast<float>(modelBounds2D.center().y()),
                                 static_cast<float>(centerz)),
                       QVector3D(0,0,1));
}
