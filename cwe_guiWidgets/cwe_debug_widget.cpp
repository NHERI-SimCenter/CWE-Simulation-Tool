#include "cwe_debug_widget.h"
#include "ui_cwe_debug_widget.h"

#include "cwe_busyindicator.h"
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>

#include "SimCenter_widgets/sctrdatawidget.h"

CWE_Debug_Widget::CWE_Debug_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CWE_Debug_Widget)
{
    ui->setupUi(this);
    theBusy = NULL;

    QJsonObject obj;
    obj.insert("name","myVariable"); // needs to be added from the config file
    obj.insert("type","std");
    obj.insert("displayname","My Variable");
    obj.insert("unit","m/s");
    obj.insert("precision","6");
    obj.insert("default","1.23");
    obj.insert("sign","+");

    ui->DataWidget->setData(obj);

    //QJsonObject obj;
    obj.insert("name","mySwitch"); // needs to be added from the config file
    obj.insert("type","bool");
    obj.insert("displayname","My Switch");
    obj.insert("unit","");
    obj.insert("precision","");
    obj.insert("default","true");
    obj.insert("sign","");

    ui->BoolWidget->setData(obj);

    //QJsonObject obj;
    obj.insert("name","myChoice"); // needs to be added from the config file
    obj.insert("type","choose");
    obj.insert("displayname","My Choice");
    obj.insert("unit","unitless");
    obj.remove("precision");
    obj.insert("default","option 3");
    obj.remove("sign");
    QJsonObject options;
    options.insert("OPT_1", "option 1");
    options.insert("OPT_2", "option 2");
    options.insert("OPT_3", "option 3");
    options.insert("OPT_X", "other");
    obj.insert("options", options);

    ui->ChooseWidget->setData(obj);
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
        ui->pushButton_2->setText("hide");
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

void CWE_Debug_Widget::on_pushButton_4_clicked()
{
    SimCenterViewState viewState = ui->BoolWidget->ViewState();
    switch (viewState) {
    case SimCenterViewState::editable:
        ui->BoolWidget->setViewState(SimCenterViewState::visible);
        break;
    case SimCenterViewState::visible:
        ui->BoolWidget->setViewState(SimCenterViewState::editable);
        break;
    case SimCenterViewState::hidden:
        ui->BoolWidget->setViewState(SimCenterViewState::visible);
        ui->pushButton_3->setText("hide");
        break;
    default:
        ui->BoolWidget->setViewState(SimCenterViewState::visible);
    }
}

void CWE_Debug_Widget::on_pushButton_3_clicked()
{
    if (ui->BoolWidget->ViewState() == SimCenterViewState::hidden) {
        ui->BoolWidget->setViewState(SimCenterViewState::visible);
        ui->pushButton_3->setText("hide");
    }
    else
    {
        ui->BoolWidget->setViewState(SimCenterViewState::hidden);
        ui->pushButton_3->setText("show");
    }
}

void CWE_Debug_Widget::on_pushButton_5_clicked()
{
    // toggle button
    SimCenterViewState viewState = ui->ChooseWidget->ViewState();
    switch (viewState) {
    case SimCenterViewState::editable:
        ui->ChooseWidget->setViewState(SimCenterViewState::visible);
        break;
    case SimCenterViewState::visible:
        ui->ChooseWidget->setViewState(SimCenterViewState::editable);
        break;
    case SimCenterViewState::hidden:
        ui->ChooseWidget->setViewState(SimCenterViewState::visible);
        ui->pushButton_6->setText("hide");
        break;
    default:
        ui->ChooseWidget->setViewState(SimCenterViewState::visible);
    }
}

void CWE_Debug_Widget::on_pushButton_6_clicked()
{
    if (ui->ChooseWidget->ViewState() == SimCenterViewState::hidden) {
        ui->ChooseWidget->setViewState(SimCenterViewState::visible);
        ui->pushButton_6->setText("hide");
    }
    else
    {
        ui->ChooseWidget->setViewState(SimCenterViewState::hidden);
        ui->pushButton_6->setText("show");
    }
}
