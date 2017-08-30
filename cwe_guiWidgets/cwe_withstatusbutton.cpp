#include "cwe_withstatusbutton.h"
#include "ui_cwe_withstatusbutton.h"
//#include <QPainter>
#include <QMouseEvent>

CWE_WithStatusButton::CWE_WithStatusButton(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_WithStatusButton)
{
    m_active = true;
    ui->setupUi(this);
    this->setInActive();
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

void CWE_WithStatusButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->setActive(true);
        this->setStyleSheet("background: #B0BEC5;");
        emit btn_pressed(m_index);
    }
}

void CWE_WithStatusButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        this->setActive();
        emit btn_released(m_index);
    }
}

void CWE_WithStatusButton::setActive(bool b)
{
    this->setStyleSheet("background: #64B5F6;");
    if (!b) this->setInActive();
    m_active = b;
}

void CWE_WithStatusButton::setInActive(bool b)
{
    this->setStyleSheet("background: #84d5Ff;");
    if (!b) this->setActive();
    m_active = !b;
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
