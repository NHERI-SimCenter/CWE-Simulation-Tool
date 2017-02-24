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

#include "taskpanelentry.h"

int TaskPanelEntry::activeFrameId = -1;
int TaskPanelEntry::nextUnusedFrameId = 0;

TaskPanelEntry::TaskPanelEntry(QObject *parent) : QObject(parent)
  , frameNameList({"Invalid Entry", "Invalid Entry"})
{
    frameId = getNewFrameId();
    implemented = true;
    fileTreeVisible = false;
    ownWidget = NULL;
}

TaskPanelEntry::~TaskPanelEntry()
{
    //TODO: Double check all the needed cleanup
    ownWidget->deleteLater();
}

void TaskPanelEntry::setFrameNameList(QStringList nameList)
{
    frameNameList = nameList;
}

void TaskPanelEntry::setAsNotImplemented()
{
    implemented = false;
}

void TaskPanelEntry::setAsActive()
{
    activeFrameId = frameId;
}

void TaskPanelEntry::setFileTreeVisibleSetting(bool newSetting)
{
    fileTreeVisible = newSetting;
}

bool TaskPanelEntry::isImplemented()
{
    return implemented;
}

bool TaskPanelEntry::isCurrentActiveFrame()
{
    return (activeFrameId == frameId);
}

bool TaskPanelEntry::fileTreeIsVisible()
{
    return fileTreeVisible;
}

int TaskPanelEntry::getFrameId()
{
    return frameId;
}

QStringList TaskPanelEntry::getFrameNames()
{
    return frameNameList;
}

QWidget * TaskPanelEntry::getOwnedWidget()
{
    if (ownWidget == NULL)
    {
        ownWidget = new QWidget();
        setupOwnFrame();
    }
    return ownWidget;
}

int TaskPanelEntry::getActiveFrameId()
{
    return activeFrameId;
}

int TaskPanelEntry::getNewFrameId()
{
    int ret = nextUnusedFrameId;
    nextUnusedFrameId++;
    return ret;
}

//These virtual functions should be overwritten
void TaskPanelEntry::setupOwnFrame()
{
    QLabel * warningLabel = new QLabel("Error, this message should never appear.");
    QHBoxLayout *hLayout = new QHBoxLayout;

    hLayout->addWidget(warningLabel);
    ownWidget->setLayout(hLayout);
}

void TaskPanelEntry::frameNowVisible()
{

}

void TaskPanelEntry::frameNowInvisible()
{

}
