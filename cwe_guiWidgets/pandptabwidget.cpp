#include "pandptabwidget.h"
#include "ui_pandptabwidget.h"
#include "cwe_parametertab.h"
#include "cwe_withstatusbutton.h"

PandPTabWidget::PandPTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PandPTabWidget)
{
    ui->setupUi(this);
}

PandPTabWidget::~PandPTabWidget()
{
    delete ui;
}

QWidget *PandPTabWidget::currentWidget()
{
    return ui->stackedWidget->currentWidget();
}

void PandPTabWidget::setCurrentWidget(QWidget *w)
{
    ui->stackedWidget->setCurrentWidget(w);
}

QWidget *PandPTabWidget::widget(int idx)
{
    return ui->stackedWidget->widget(idx);
}

int PandPTabWidget::addTab(QWidget *page, const QString &label)
{
    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText("Something\nFunny\nParameters");
    newTab->setStatus("* unknown *");
    int row = ui->verticalTabLayout->count()-1;
    ui->verticalTabLayout->insertWidget(row, newTab);

    // create the widget to hold the parameter input
    QTabWidget *pWidget = new QTabWidget();
    ui->stackedWidget->insertWidget(row,pWidget);

    QFrame *itm = new QFrame();
    itm->setStyleSheet("QFrame {background: red}");
    pWidget->addTab(itm, "something");
    QVBoxLayout *lyt = new QVBoxLayout();
    lyt->addWidget(itm);
    pWidget->setLayout(lyt);

    return row;
}

void PandPTabWidget::on_pbtn_run_clicked()
{

}

void PandPTabWidget::on_pbtn_cancel_clicked()
{

}

void PandPTabWidget::on_pbtn_results_clicked()
{

}

void PandPTabWidget::on_pbtn_rollback_clicked()
{

}
