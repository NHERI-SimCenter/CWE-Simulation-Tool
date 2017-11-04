#include "sctrdatawidget.h"
#include "ui_sctrdatawidget.h"

SCtrDataWidget::SCtrDataWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SCtrDataWidget)
{
    ui->setupUi(this);
    this->setViewState(SimCenterViewState::visible);
}

SCtrDataWidget::~SCtrDataWidget()
{
    delete ui;
}

SimCenterViewState SCtrDataWidget::ViewState()
{
    return m_ViewState;
}

void SCtrDataWidget::setViewState(SimCenterViewState state)
{
    switch (state) {
    case SimCenterViewState::visible:
        ui->txted_theValue->setEnabled(false);
        this->show();
        m_ViewState = state;
        break;
    case SimCenterViewState::editable:
        ui->txted_theValue->setEnabled(true);
        this->show();
        m_ViewState = state;
        break;
    case SimCenterViewState::hidden:
        ui->txted_theValue->setEnabled(false);
        this->hide();
        m_ViewState = state;
        break;
    default:
        m_ViewState = SimCenterViewState::visible;
    }
}

void SCtrDataWidget::setData(QJsonObject &obj)
{

}

void SCtrDataWidget::setValue(QString s)
{
    ui->txted_theValue->setText(s);
}

void SCtrDataWidget::setValue(float v)
{
    QString s = QString("%f").arg(v);
    ui->txted_theValue->setText(s);
}

void SCtrDataWidget::setValue(int i)
{
    QString s = QString("%d").arg(i);
    ui->txted_theValue->setText(s);
}

void SCtrDataWidget::setValue(bool b)
{
    QString s = b?"true":"false";
    ui->txted_theValue->setText(s);
}

QString SCtrDataWidget::Value()
{
    QString s = ui->txted_theValue->toPlainText();
}

/* ********** SLOTS ********** */
void SCtrDataWidget::on_txted_theValue_textChanged()
{
    // validator
}
