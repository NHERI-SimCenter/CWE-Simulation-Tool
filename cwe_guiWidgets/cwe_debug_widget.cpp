#include "cwe_debug_widget.h"
#include "ui_cwe_debug_widget.h"

#include "cwe_busyindicator.h"
#include <QTimer>

#include "SimCenter_widgets/sctrdatawidget.h"

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


/* ********** SLOTS ********** */

void CWE_Debug_Widget::on_pushButton_clicked()
{
    SimCenterViewState viewState = ui->DataWidget->ViewState();
    switch (viewState) {
    case SimCenterViewState::editable:
        ui->DataWidget->setViewState(SimCenterViewState::visible);
        break;
    case SimCenterViewState::visible:
        ui->DataWidget->setViewState(SimCenterViewState::editable);
        break;
    case SimCenterViewState::hidden:
        ui->DataWidget->setViewState(SimCenterViewState::visible);
        break;
    default:
        ui->DataWidget->setViewState(SimCenterViewState::visible);
    }
}

void CWE_Debug_Widget::on_pushButton_2_clicked()
{
    if (ui->DataWidget->ViewState() == SimCenterViewState::hidden) {
        ui->DataWidget->setViewState(SimCenterViewState::visible);
        ui->pushButton_2->setText("hide");
    }
    else
    {
        ui->DataWidget->setViewState(SimCenterViewState::hidden);
        ui->pushButton_2->setText("show");
    }
}
