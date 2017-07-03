#ifndef REMOTEJOBLISTER_H
#define REMOTEJOBLISTER_H

#include <QListView>
#include <QMenu>

class JobOperator;

class RemoteJobLister : public QListView
{
    Q_OBJECT
public:
    explicit RemoteJobLister(QWidget *parent = nullptr);
    void setJobHandle(JobOperator * theJobHandle);

private slots:
    void needRightClickMenu(QPoint);

private:
    JobOperator * myJobHandle = NULL;
};

#endif // REMOTEJOBLISTER_H
