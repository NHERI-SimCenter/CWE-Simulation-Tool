#include "pandptabwidget.h"
#include "ui_pandptabwidget.h"
#include "cwe_parametertab.h"
#include "cwe_withstatusbutton.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

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

    groupWidget     = new QMap<QString, CWE_WithStatusButton *>();
    groupTabList    = new QMap<QString, QTabWidget *>();
    variableWidgets = new QMap<QString, QWidget *>();
    varTabWidgets   = new QMap<QString, QMap<QString, QWidget *> *>();
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
    varTabWidgets->insert(key, new QMap<QString, QWidget *>());

    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText(label);
    newTab->setStatus("* unknown *");
    int index = ui->verticalTabLayout->count()-1;
    newTab->setIndex(index);
    ui->verticalTabLayout->insertWidget(index, newTab);

    groupWidget->insert(key, newTab);

    connect(newTab,SIGNAL(btn_pressed(int)),this,SLOT(on_groupTabSelected(int)));
    //connect(newTab,SIGNAL(btn_released(int)),this,SLOT(on_groupTabSelected(int)));

    // create the widget to hold the parameter input
    QTabWidget *pWidget = new QTabWidget();
    ui->stackedWidget->insertWidget(index, pWidget);

    groupTabList->insert(key, pWidget);

    // EXAMPLES: how to add tabs ...
    //this->addVarTab(key, tr("%1 game").arg(key));
    //this->addVarTab(key, tr("%1 on!").arg(key));

    return index;
}

int PandPTabWidget::addVarTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo)
{
    int index = addVarTab(key, label);
    if (index >= 0)
    {
        addVarsToTab(key, label, varList, varsInfo);
    }
}

void PandPTabWidget::addVarsToTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo)
{
    //QTabWidget *groupTab = groupTabList->value(key);
    //QWidget    *varTab   = varTabWidgets->value(key)->value(label);

    foreach (const QJsonValue &item, *varList)
    {
        QString varKey = item.toString();
        QJsonObject variableObject = (*varsInfo)[varKey].toObject();
        this->addVariable(variableObject, key, label);
    }
    this->addVSpacer(key, label);
}

int PandPTabWidget::addVarTab(QString key, const QString &label)
{
    // create the widget to hold the parameter input

    QScrollArea *itm = new QScrollArea();
    itm->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //itm->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //itm->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);

    QGridLayout *lyt = new QGridLayout();
    itm->setLayout(lyt);

    varTabWidgets->value(key)->insert(label, itm);
    //itm->setStyleSheet("QFrame {background: lightblue}");  // this works, so why doesn't the style sheet work?

    QTabWidget * qf = groupTabList->value(key);
    int index = qf->addTab(itm, label);

    return index;
}

void PandPTabWidget::addVarsData(QJsonObject JSONgroup, QJsonObject JSONvars)
{

}

void PandPTabWidget::addStd(QJsonObject JSONvar, QWidget *parent)
{
    QVariant defaultOption = JSONvar["default"].toVariant();
    QString unit           = JSONvar["unit"].toString();

    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);
    theName->setMinimumHeight(16);
    //theName->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    QDoubleSpinBox *theValue = new QDoubleSpinBox(parent);
    theValue->setValue(defaultOption.toDouble());

    QLabel *theUnit = new QLabel(parent);
    theUnit->setText(unit);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theValue,row,1);
    layout->addWidget(theUnit, row,2);
}

void PandPTabWidget::addBool(QJsonObject JSONvar, QWidget *parent)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QCheckBox *theBox = new QCheckBox(parent);
    theBox->setChecked(JSONvar["default"].toBool());

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theBox, row,1);
}

void PandPTabWidget::addFile(QJsonObject JSONvar, QWidget *parent)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QLineEdit *theFileName = new QLineEdit(parent);
    theFileName->setText("unknown file name");

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theFileName,row,1,1,2);
}

void PandPTabWidget::addChoice(QJsonObject JSONvar, QWidget *parent)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QComboBox *theSelection = new QComboBox(parent);
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

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theSelection,row,1,1,2);
}

void PandPTabWidget::addUnknown(QJsonObject JSONvar, QWidget *parent)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
}

void PandPTabWidget::addType(const QString type, QJsonObject JSONvar, QWidget *parent)
{
    if (type == "std") { this->addStd(JSONvar, parent); }
    else if (type == "bool") { this->addBool(JSONvar, parent); }
    else if (type == "file") { this->addFile(JSONvar, parent); }
    else if (type == "choose") { this->addChoice(JSONvar, parent); }
    else { this->addUnknown(JSONvar, parent); }
}

bool PandPTabWidget::addVariable(QJsonObject JSONvar, const QString &key, const QString &label)
{
    QString type = JSONvar["type"].toString();
    if (type == "") {
        return false;
    }
    else
    {
        QWidget *parent = varTabWidgets->value(key)->value(label);
        if (parent != NULL)
        {
            this->addType(type, JSONvar, parent);
            return true;
        }
        else { return false; }
    }
}

void PandPTabWidget::addVSpacer(const QString &key, const QString &label)
{
    QWidget *parent = varTabWidgets->value(key)->value(label);
    if (parent != NULL)
    {
        QGridLayout *layout = (QGridLayout*)(parent->layout());
        layout->addItem(new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), layout->rowCount(), 2);
    }
}

void PandPTabWidget::setIndex(int idx)
{
    // set active tab to idx
    ui->stackedWidget->setCurrentIndex(idx);
    activeIndex   = ui->stackedWidget->currentIndex();
    displayWidget = ui->stackedWidget->currentWidget();

    // set stylesheet for buttons
    foreach (const QString &key, groupWidget->keys())
    {
        CWE_WithStatusButton *btn = groupWidget->value(key);
        //qDebug() << idx << "<>" << btn->index();

        if (btn->index() == idx)
            { btn->setActive(); }
        else
            { btn->setInActive(); }
    }
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

void PandPTabWidget::on_groupTabSelected(int idx)
{
    this->setIndex(idx);
}
