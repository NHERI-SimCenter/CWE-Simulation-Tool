#include "pandptabwidget.h"
#include "ui_pandptabwidget.h"
#include "cwe_parametertab.h"
#include "cwe_withstatusbutton.h"

#include <QJsonObject>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QLayout>

#include <QList>

#include "qdebug.h"

PandPTabWidget::PandPTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PandPTabWidget)
{
    ui->setupUi(this);

    groupWidget  = new QMap<QString, QWidget *>();
    groupTabList = new QMap<QString, QList<QWidget *> *>();
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

int PandPTabWidget::addGroupTab(QString key, const QString &label)
{
    QList<QWidget *> *tabList = new QList<QWidget *>();
    groupTabList->insert(key, tabList);

    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText(label);
    newTab->setStatus("* unknown *");
    int index = ui->verticalTabLayout->count()-1;
    ui->verticalTabLayout->insertWidget(index, newTab);

    // create the widget to hold the parameter input
    QTabWidget *pWidget = new QTabWidget();
    pWidget->setStyleSheet("background: green");
    ui->stackedWidget->insertWidget(index, pWidget);

    groupWidget->insert(key, pWidget);

    this->addVarTab(key, "game");
    this->addVarTab(key, "on!");

    return index;
}

int PandPTabWidget::addVarTab(QString key, const QString &label)
{
    qDebug() << "PandPTabWidget::addVarTab" << label;

    // create the widget to hold the parameter input
    //ui->stackedWidget->insertWidget(index,page);

    QFrame *itm = new QFrame();
    itm->setStyleSheet("QFrame {background: lightblue}");
    QTabWidget * qf = (QTabWidget *)groupWidget->value(key);
    int index = qf->addTab(itm, label);

    QList<QWidget *> *ql = (QList<QWidget *> *)groupTabList->value(key);
    ql->append(itm);

    return index;
}

void PandPTabWidget::addVarsData(QJsonObject JSONgroup, QJsonObject JSONvars)
{

}

void PandPTabWidget::addStd(QJsonObject JSONvar)
{
    QVariant defaultOption = JSONvar["default"].toVariant();
    QString unit           = JSONvar["unit"].toString();

    QLabel *theName = new QLabel(displayWidget);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QDoubleSpinBox *theValue = new QDoubleSpinBox(displayWidget);
    theValue->setValue(defaultOption.toDouble());

    QLabel *theUnit = new QLabel(displayWidget);
    theUnit->setText(unit);

    QGridLayout *layout = (QGridLayout*)(displayWidget->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theValue,row,1);
    layout->addWidget(theUnit, row,2);
}

void PandPTabWidget::addBool(QJsonObject JSONvar)
{
    QLabel *theName = new QLabel(displayWidget);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QCheckBox *theBox = new QCheckBox(displayWidget);
    theBox->setChecked(JSONvar["default"].toBool());

    QGridLayout *layout = (QGridLayout*)(displayWidget->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theBox, row,1);
}

void PandPTabWidget::addFile(QJsonObject JSONvar)
{
    QLabel *theName = new QLabel(displayWidget);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QLineEdit *theFileName = new QLineEdit(displayWidget);
    theFileName->setText("unknown file name");

    QGridLayout *layout = (QGridLayout*)(displayWidget->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theFileName,row,1,1,2);
}

void PandPTabWidget::addChoice(QJsonObject JSONvar)
{
    QLabel *theName = new QLabel(displayWidget);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QComboBox *theSelection = new QComboBox(displayWidget);
    QJsonObject combo_options = JSONvar["options"].toObject();

    QStandardItemModel *theModel = new QStandardItemModel();
    foreach (const QString &theKey, combo_options.keys())
    {
        //QStandardItem *itm = new QStandardItem(theKey);
        QStandardItem *itm = new QStandardItem(combo_options[theKey].toString());
        theModel->appendRow(itm);
    }
    theSelection->setModel(theModel);
    theSelection->setCurrentText(combo_options[JSONvar["default"].toString()].toString());

    QGridLayout *layout = (QGridLayout*)(displayWidget->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theSelection,row,1,1,2);
}

void PandPTabWidget::addUnknown(QJsonObject JSONvar)
{
    QLabel *theName = new QLabel(displayWidget);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QGridLayout *layout = (QGridLayout*)(displayWidget->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
}

void PandPTabWidget::addType(const QString type, QJsonObject JSONvar)
{
    if (type == "std") { this->addStd(JSONvar); }
    else if (type == "bool") { this->addBool(JSONvar); }
    else if (type == "file") { this->addFile(JSONvar); }
    else if (type == "choose") { this->addChoice(JSONvar); }
    else { this->addUnknown(JSONvar); }
}

void PandPTabWidget::setIndex(int idx)
{
    // set active tab to idx
    ui->stackedWidget->setCurrentIndex(idx);
    activeIndex   = ui->stackedWidget->currentIndex();
    displayWidget = ui->stackedWidget->currentWidget();
}

void PandPTabWidget::setWidget(QWidget *w)
{
    // set active tab to idx
    ui->stackedWidget->setCurrentWidget(w);
    activeIndex   = ui->stackedWidget->currentIndex();
    displayWidget = ui->stackedWidget->currentWidget();
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
