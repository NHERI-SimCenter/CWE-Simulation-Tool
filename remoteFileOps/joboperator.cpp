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

#include "joboperator.h"

#include "remotefiletree.h"
#include "../AgaveClientInterface/remotedatainterface.h"

JobOperator::JobOperator(RemoteDataInterface * newDataLink, QListView * newJobList, RemoteFileWindow * parent) : QObject((QObject *)parent)
{
    myJobListView = newJobList;
    myJobListView->setModel(&theJobList);
    dataLink = newDataLink;
    myFileWindow = parent;
    QObject::connect(dataLink, SIGNAL(longRunningTasksUpdated()), this, SLOT(refreshRunningJobList()));
    QObject::connect(myJobListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(needRightClickMenu(QPoint)));
}

void JobOperator::refreshRunningJobList()
{
    theJobList.clear();
    QList<LongRunningTask *> updatedTaskList = dataLink->getListOfLongTasks();
    theJobList.setColumnCount(2);

    for (auto itr = updatedTaskList.cbegin(); itr != updatedTaskList.cend(); itr++)
    {
        QList<QStandardItem *> newRow;
        newRow.append(new QStandardItem((*itr)->getRawDataStr()));
        newRow.append(new QStandardItem((*itr)->getIDstr()));
        theJobList.appendRow(newRow);
    }
}

void JobOperator::needRightClickMenu(QPoint)
{
    QMenu jobMenu;

    jobMenu.addAction("Refresh Info", this, SLOT(demandJobDataRefresh()));

    jobMenu.exec(QCursor::pos());
}

void JobOperator::demandJobDataRefresh()
{
    dataLink->forceRefreshOfLongTasks();
}
