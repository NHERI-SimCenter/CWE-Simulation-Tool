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

#include "cfdglcanvas.h"

#include "cfdtoken.h"

CFDglCanvas::CFDglCanvas(QWidget *parent, Qt::WindowFlags f) : QOpenGLWidget(parent,f) {}

CFDglCanvas::~CFDglCanvas()
{
    clearAllData();
}

bool CFDglCanvas::loadFieldData(QByteArray * rawDataFile, QString valueType)
{
    CFDtoken * dataRoot = CFDtoken::lexifyString(rawDataFile);

    if (!CFDtoken::parseTokenStream(dataRoot))
    {
        currentDisplayError = "Unable to read data file";
        delete dataRoot;
        return false;
    }

    CFDtoken * dataElement = dataRoot->getLargestChildArray();

    if (dataElement == nullptr)
    {
        currentDisplayError = "Unable to locate data in data file";
        delete dataRoot;
        return false;
    }

    if (valueType == "scalar")
    {
        for (auto itr = dataElement->getChildList().cbegin();
             itr != dataElement->getChildList().cend(); itr++)
        {
            if (((*itr)->getType() == CFDtokenType::FLOAT) ||
                    ((*itr)->getType() == CFDtokenType::INT))
            {
                dataList.append((*itr)->getFloatVal());
            }
            else
            {
                currentDisplayError = "Data list does not contain floats";
                delete dataRoot;
                return false;
            }
        }
    }
    else if (valueType == "magnitude")
    {
        for (auto itr = dataElement->getChildList().cbegin();
             itr != dataElement->getChildList().cend(); itr++)
        {
            if ((*itr)->getType() != CFDtokenType::DATA_ARRAY)
            {
                currentDisplayError = "Data list does not contain float arrays";
                delete dataRoot;
                return false;
            }

            double sum = 0.0;
            for (auto itr2 = (*itr)->getChildList().cbegin();
                 itr2 != (*itr)->getChildList().cend(); itr2++)
            {
                double rawVal = (*itr2)->getFloatVal();
                sum += rawVal * rawVal;
            }
            dataList.append(sqrt(sum));
        }
    }
    else
    {
        currentDisplayError = "Invalid data type";

        delete dataRoot;
        return false;
    }

    QList<double> sortedList = dataList;

    std::sort(sortedList.begin(), sortedList.end());

    if (sortedList.size() < 50)
    {
        lowDataVal = sortedList.at(0);
        highDataVal = sortedList.last();
    }
    else
    {
        lowDataVal = sortedList.at(19);
        highDataVal = sortedList.at(sortedList.size()-19);
    }

    delete dataRoot;
    return true;
}

bool CFDglCanvas::displayAvailData()
{
    if (!currentDisplayError.isEmpty()) return false;
    if (pointList.isEmpty()) return false;
    if (faceList.isEmpty()) return false;
    if (ownerList.isEmpty()) return false;
    readyToDisplay = true;
    recomputePerspecMat();
    recomputeViewModelMat();
    this->update();
    return true;
}

QString CFDglCanvas::getDisplayError()
{
    return currentDisplayError;
}

void CFDglCanvas::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void CFDglCanvas::resizeGL(int w, int h)
{
    myDisplayWidth = w;
    myDisplayHeight = h;
    recomputePerspecMat();
}

bool CFDglCanvas::isAllZ0(QList<int> aFace)
{
    for (auto pointIndItr = aFace.cbegin(); pointIndItr != aFace.cend(); pointIndItr++)
    {
        if (pointList.at(*pointIndItr).at(2) > PRECISION)
        {
            return false;
        }
        if (pointList.at(*pointIndItr).at(2) < -PRECISION)
        {
            return false;
        }
    }
    return true;
}

bool CFDglCanvas::loadRawMeshData(QByteArray * rawPointFile, QByteArray * rawFaceFile, QByteArray * rawOwnerFile)
{
    clearAllData();

    CFDtoken * pointRoot = CFDtoken::lexifyString(rawPointFile);
    CFDtoken * faceRoot = CFDtoken::lexifyString(rawFaceFile);
    CFDtoken * ownerRoot = CFDtoken::lexifyString(rawOwnerFile);

    if (!CFDtoken::parseTokenStream(pointRoot) ||
        !CFDtoken::parseTokenStream(faceRoot) ||
        !CFDtoken::parseTokenStream(ownerRoot))
    {
        currentDisplayError = "Unable to read mesh data files";
        delete pointRoot;
        delete faceRoot;
        delete ownerRoot;
        return false;
    }

    CFDtoken * pointElement = pointRoot->getLargestChildArray();
    CFDtoken * faceElement = faceRoot->getLargestChildArray();
    CFDtoken * ownerElement = ownerRoot->getLargestChildArray();

    if ((pointElement == nullptr) || (faceElement == nullptr) || (ownerElement == nullptr))
    {
        currentDisplayError = "Unable to locate mesh data in files";
        delete pointRoot;
        delete faceRoot;
        delete ownerRoot;
        return false;
    }

    //TODO: Add more validity checks before reading each element
    for (auto itr = pointElement->getChildList().cbegin();
         itr != pointElement->getChildList().cend(); itr++)
    {
        if ((*itr)->getChildSize() != 3)
        {
            currentDisplayError = "Point list does not contain points";
            delete pointRoot; delete faceRoot; delete ownerRoot;
            return false;
        }
        QList<double> aPoint;

        for (auto coordItr = (*itr)->getChildList().cbegin();
             coordItr != (*itr)->getChildList().cend(); coordItr++)
        {
            if ((*coordItr)->getType() == CFDtokenType::INT)
            {
                double tmp = static_cast<double> ((*coordItr)->getIntVal());
                aPoint.append(tmp);
            }
            else if ((*coordItr)->getType() == CFDtokenType::FLOAT)
            {
                aPoint.append((*coordItr)->getFloatVal());
            }
            else
            {
                currentDisplayError = "Point list does not contain numbers";
                delete pointRoot; delete faceRoot; delete ownerRoot;
                return false;
            }
        }

        pointList.append(aPoint);
    }

    for (auto itr = faceElement->getChildList().cbegin();
         itr != faceElement->getChildList().cend(); itr++)
    {
        QList<int> aFace;

        for (auto elementItr = (*itr)->getChildList().cbegin();
             elementItr != (*itr)->getChildList().cend(); elementItr++)
        {
            if ((*elementItr)->getType() == CFDtokenType::INT)
            {
                aFace.append((*elementItr)->getIntVal());
            }
            else
            {
                currentDisplayError = "Face list does not contain ints";
                delete pointRoot; delete faceRoot; delete ownerRoot;
                return false;
            }
        }

        faceList.append(aFace);
    }

    for (auto itr = ownerElement->getChildList().cbegin();
         itr != ownerElement->getChildList().cend(); itr++)
    {
        if ((*itr)->getType() == CFDtokenType::INT)
        {
            ownerList.append((*itr)->getIntVal());
        }
        else
        {
            currentDisplayError = "Owner list does not contain ints";
            delete pointRoot; delete faceRoot; delete ownerRoot;
            return false;
        }
    }

    delete pointRoot;
    delete faceRoot;
    delete ownerRoot;

    modelBounds2D.setBottom(pointList.at(0).at(1));
    modelBounds2D.setTop(pointList.at(0).at(1));
    modelBounds2D.setLeft(pointList.at(0).at(0));
    modelBounds2D.setRight(pointList.at(0).at(0));

    for (auto itr = pointList.cbegin(); itr != pointList.cend(); itr++)
    {
        double xVal = (*itr).at(0);
        double yVal = (*itr).at(1);

        if (xVal < modelBounds2D.left()) modelBounds2D.setLeft(xVal);
        if (xVal > modelBounds2D.right()) modelBounds2D.setRight(xVal);
        if (yVal > modelBounds2D.top()) modelBounds2D.setTop(yVal);
        if (yVal < modelBounds2D.bottom()) modelBounds2D.setBottom(yVal);
    }

    return true;
}

void CFDglCanvas::clearAllData()
{
    currentDisplayError.clear();

    pointList.clear();
    faceList.clear();
    ownerList.clear();

    dataList.clear();
}
