#ifndef COPYRIGHTDIALOG_H
#define COPYRIGHTDIALOG_H

#include <QDialog>

namespace Ui {
class CopyrightDialog;
}

class CopyrightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CopyrightDialog(QWidget *parent = 0);
    ~CopyrightDialog();

private:
    Ui::CopyrightDialog *ui;
};

#endif // COPYRIGHTDIALOG_H
