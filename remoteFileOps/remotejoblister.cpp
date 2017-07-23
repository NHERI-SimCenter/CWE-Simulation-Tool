#include "remotejoblister.h"
#include "joboperator.h"

RemoteJobLister::RemoteJobLister(QWidget *parent) : QListView(parent)
{
    QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(needRightClickMenu(QPoint)));
}

void RemoteJobLister::setJobHandle(JobOperator * theJobHandle)
{
    myJobHandle = theJobHandle;
    myJobHandle->linkToJobLister(this);
}

void RemoteJobLister::needRightClickMenu(QPoint)
{
    if (myJobHandle == NULL)
    {
        return;
    }
    QMenu jobMenu;

    jobMenu.addAction("Refresh Info", myJobHandle, SLOT(demandJobDataRefresh()));
    jobMenu.exec(QCursor::pos());
}
