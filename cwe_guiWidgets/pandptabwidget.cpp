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
    // identify the parent
    QWidget * container = ui->stackedWidget->currentWidget();

    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText("Something\nFunny\nParameters");
    newTab->setStatus("* unknown *");
    int index = ui->verticalTabLayout->count()-1;
    //ui->verticalTabLayout->insertWidget(index, newTab);

    // create the widget to hold the parameter input
    //QTabWidget *pWidget = new QTabWidget();

    //  QWidget * parent = ui->stackedWidget->widget();

    //QFrame *itm = new QFrame();
    //itm->setStyleSheet("QFrame {background: red}");
    //pWidget->addTab(itm, "something");
    //QVBoxLayout *lyt = new QVBoxLayout();
    //lyt->addWidget(itm);
    //pWidget->setLayout(lyt);

    return index;
}

int PandPTabWidget::addMasterTab(QWidget *page, const QString &label)
{
    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText(label);
    newTab->setStatus("* unknown *");
    int index = ui->verticalTabLayout->count()-1;
    ui->verticalTabLayout->insertWidget(index, newTab);

    // create the widget to hold the parameter input
    ui->stackedWidget->insertWidget(index,page);

    return index;
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
