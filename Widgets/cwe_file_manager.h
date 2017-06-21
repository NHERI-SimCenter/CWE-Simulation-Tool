#ifndef CWE_FILE_MANAGER_H
#define CWE_FILE_MANAGER_H

#include <QWidget>

namespace Ui {
class CWE_file_manager;
}

class CWE_file_manager : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_file_manager(QWidget *parent = 0);
    ~CWE_file_manager();

private:
    Ui::CWE_file_manager *ui;
};

#endif // CWE_FILE_MANAGER_H
