#include "cwe_withstatusbutton.h"
#include "ui_cwe_withstatusbutton.h"
//#include <QPainter>

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


#if 0
void CWE_WithStatusButton::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    QPen myPen;
    myPen.setWidth(2);

    if(isChecked())
    {
        myPen.setColor(Qt::black);
        painter.setPen(myPen);
        painter.drawLine(20,29,50,20);
        int a = 1;
        emit valueChanged(a);
    }
    else
    {
        myPen.setColor(Qt::darkGray);
        painter.setPen(myPen);
        painter.drawLine(20,29,30,0);
        int b = 0;
        emit valueChanged(b);
    }
    painter.drawLine(0,30,20,30);
    painter.drawLine(50,30,70,30);
    painter.drawLine(50,30,50,20);
}
#endif
