#include "cwe_debug_widget.h"
#include "ui_cwe_debug_widget.h"

CWE_Debug_Widget::CWE_Debug_Widget(QWidget *parent) :
    CWE_Super(parent),
    ui(new Ui::CWE_Debug_Widget)
{
    ui->setupUi(this);
}

CWE_Debug_Widget::~CWE_Debug_Widget()
{
    delete ui;
}
