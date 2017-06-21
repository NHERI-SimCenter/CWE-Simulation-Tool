#ifndef CWE_TASK_LIST_H
#define CWE_TASK_LIST_H

#include <QWidget>

namespace Ui {
class CWE_task_list;
}

class CWE_task_list : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_task_list(QWidget *parent = 0);
    ~CWE_task_list();

private:
    Ui::CWE_task_list *ui;
};

#endif // CWE_TASK_LIST_H
