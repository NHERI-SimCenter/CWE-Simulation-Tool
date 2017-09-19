#include "cwe_debug_widget.h"
#include "ui_cwe_debug_widget.h"

#include "cwe_busyindicator.h"
#include <QTimer>

CWE_Debug_Widget::CWE_Debug_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Debug_Widget)
{
    ui->setupUi(this);
    theBusy = NULL;
}

CWE_Debug_Widget::~CWE_Debug_Widget()
{
    delete ui;
}

void CWE_Debug_Widget::on_pb_setBusy_clicked()
{
    if (theBusy == NULL) { theBusy = new CWE_BusyIndicator(); }
    theBusy->show();
}

void CWE_Debug_Widget::on_pb_stopBusy_clicked()
{
    if (theBusy != NULL)
    {
        theBusy->hide();
        // theBusy = NULL;
    }
}

void CWE_Debug_Widget::on_pb_timedBusy_clicked()
{
    if (theBusy == NULL) { theBusy = new CWE_BusyIndicator(); }
    theBusy->show();
    QTimer::singleShot(2000, this, SLOT(on_pb_stopBusy_clicked()));
}
