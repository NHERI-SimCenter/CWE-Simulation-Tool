#ifndef CWE_DEBUG_WIDGET_H
#define CWE_DEBUG_WIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>

#include "cwe_super.h"

namespace Ui {
class CWE_Debug_Widget;
}

class CWE_Debug_Widget : public CWE_Super
{
    Q_OBJECT

public:
    explicit CWE_Debug_Widget(QWidget *parent = 0);
    ~CWE_Debug_Widget();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

private:
    Ui::CWE_Debug_Widget *ui;
};

#endif // CWE_DEBUG_WIDGET_H
