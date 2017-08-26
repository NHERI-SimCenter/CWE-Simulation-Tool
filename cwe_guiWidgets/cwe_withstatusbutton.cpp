#include "cwe_withstatusbutton.h"
#include "ui_cwe_withstatusbutton.h"

CWE_WithStatusButton::CWE_WithStatusButton(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_WithStatusButton)
{
    ui->setupUi(this);
    this->setSelected(false);
}

CWE_WithStatusButton::~CWE_WithStatusButton()
{
    delete ui;
}

void CWE_WithStatusButton::setStatus(QString s)
{
    m_status = s;
    ui->statusLabel->setText(s);
}

void CWE_WithStatusButton::setText(QString s)
{
    m_text = s;
    ui->mainLabel->setText(s);
}

void CWE_WithStatusButton::on_statusLabel_linkActivated(const QString &link)
{
    this->setSelected(true);
    emit btn_clicked();
}

void CWE_WithStatusButton::on_mainLabel_linkActivated(const QString &link)
{
    this->setSelected(true);
    emit btn_clicked();
}

void CWE_WithStatusButton::setSelected(bool state)
{
    selected = state;
    if (selected)
    {
        this->setStyleSheet("QFrame {background: #B0BEC5;}");
    }
    else
    {
        this->setStyleSheet("QFrame {background: #64B5F6;}");
    }
}
