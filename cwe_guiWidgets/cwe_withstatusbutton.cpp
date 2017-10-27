#include "cwe_withstatusbutton.h"
#include "ui_cwe_withstatusbutton.h"
//#include <QPainter>
#include <QMouseEvent>

CWE_WithStatusButton::CWE_WithStatusButton(QString stageName, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_WithStatusButton)
{
    //m_active = true;
    internal_name = stageName;
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
        this->setStyleSheet("QFrame {background: #B0BEC5;}");
        emit btn_pressed(m_index, internal_name);
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
    this->setStyleSheet("QFrame {background: #64B5F6; border-color: #808080; border-width: 1.5px; border-radius: 3px; border-style: inset;} QLabel {border-style: none}");
    if (!b) this->setInActive();
    m_active = b;
}

void CWE_WithStatusButton::setInActive(bool b)
{
    this->setStyleSheet("QFrame {background: #C0C0C8; border-color: #808080; border-width: 2px; border-radius: 3px; border-style: onset;} QLabel {border-style: none}");
    if (!b) this->setActive();
    m_active = !b;
}
