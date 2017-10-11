#include "pandptabwidget.h"
#include "ui_pandptabwidget.h"
#include "cwe_parametertab.h"
#include "cwe_withstatusbutton.h"
#include "CFDanalysis/CFDcaseInstance.h"

#include "qdebug.h"

PandPTabWidget::PandPTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PandPTabWidget)
{
    ui->setupUi(this);

    groupWidget     = new QMap<QString, CWE_WithStatusButton *>();
    groupTabList    = new QMap<QString, QTabWidget *>();
    varTabWidgets   = new QMap<QString, QMap<QString, QWidget *> *>();
    variableWidgets = new QMap<QString, InputDataType *>();

    this->setButtonMode(CWE_BTN_ALL);
    //this->setButtonMode(CWE_BTN_NONE);
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

int PandPTabWidget::addGroupTab(QString key, const QString &label, StageState currentState)
{
    varTabWidgets->insert(key, new QMap<QString, QWidget *>());

    // create the tab
    CWE_WithStatusButton *newTab = new CWE_WithStatusButton();
    newTab->setText(label);

    newTab->setStatus(getStateText(currentState));
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

int PandPTabWidget::addVarTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString,QString> * setVars)
{
    int index = addVarTab(key, label);
    if (index >= 0)
    {
        addVarsToTab(key, label, varList, varsInfo, setVars);
    }
    return index;
}

void PandPTabWidget::addVarsToTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString,QString> * setVars)
{
    //QTabWidget *groupTab = groupTabList->value(key);
    //QWidget    *varTab   = varTabWidgets->value(key)->value(label);

    foreach (const QJsonValue &item, *varList)
    {
        QString varKey = item.toString();
        QJsonObject variableObject = (*varsInfo)[varKey].toObject();
        QString setVal;

        if (setVars->contains(varKey))
        {
            setVal = setVars->value(varKey);
            this->addVariable(varKey, variableObject, key, label, &setVal);
        }
        else
        {
            this->addVariable(varKey, variableObject, key, label, NULL);
        }
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

QWidget * PandPTabWidget::addStd(QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QVariant defaultOption = JSONvar["default"].toVariant();
    QString unit           = JSONvar["unit"].toString();
    // QJson fails to convert "1" to int, thus: QString::toInt( QJson::toString() )
    int precision          = JSONvar["precision"].toString().toInt();

    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);
    theName->setMinimumHeight(16);
    //theName->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    QWidget * theValue;

    if (precision > 0) {
        theValue = new QDoubleSpinBox(parent);
        ((QDoubleSpinBox *)theValue)->setValue(defaultOption.toDouble());
        ((QDoubleSpinBox *)theValue)->setDecimals(precision);
    }
    else {
        theValue = new QSpinBox(parent);
        ((QSpinBox *)theValue)->setValue(defaultOption.toInt());
    }

    QLabel *theUnit = new QLabel(parent);
    theUnit->setText(unit);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theValue,row,1);
    layout->addWidget(theUnit, row,2);

    return theValue;
}

QWidget * PandPTabWidget::addBool(QJsonObject JSONvar, QWidget *parent, QString * setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QCheckBox *theBox = new QCheckBox(parent);
    if (setVal == NULL)
    {
        theBox->setChecked(JSONvar["default"].toBool());
    }
    else
    {
        theBox->setChecked(setVal);
    }

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);
    layout->addWidget(theBox, row,1);

    return theBox;
}

QWidget * PandPTabWidget::addFile(QJsonObject JSONvar, QWidget *parent, QString * setVal)
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

    return theFileName;
}

QWidget * PandPTabWidget::addChoice(QJsonObject JSONvar, QWidget *parent, QString * setVal)
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
    if ((setVal == NULL) || (!combo_options.contains(*setVal)))
    {
        theSelection->setCurrentText(combo_options[JSONvar["default"].toString()].toString());
    }
    else
    {
        theSelection->setCurrentText(combo_options[*setVal].toString());
    }

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName, row,0);
    layout->addWidget(theSelection,row,1,1,2);

    return theSelection;
}


QWidget * PandPTabWidget::addVector3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * PandPTabWidget::addVector2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * PandPTabWidget::addTensor3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * PandPTabWidget::addTensor2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QWidget * PandPTabWidget::addUnknown(QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);

    return NULL;
}

void PandPTabWidget::addType(const QString &varName, const QString &type, QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QWidget *widget;

    widget = NULL;

    if      (type == "std")      { widget = this->addStd(JSONvar, parent, setVal); }
    else if (type == "bool")     { widget = this->addBool(JSONvar, parent, setVal); }
    else if (type == "file")     { widget = this->addFile(JSONvar, parent, setVal); }
    else if (type == "choose")   { widget = this->addChoice(JSONvar, parent, setVal); }
    else if (type == "vector2D") { widget = this->addVector2D(JSONvar, parent, setVal); }
    else if (type == "tensor2D") { widget = this->addTensor2D(JSONvar, parent, setVal); }
    else if (type == "vector3D") { widget = this->addVector3D(JSONvar, parent, setVal); }
    else if (type == "tensor3D") { widget = this->addTensor3D(JSONvar, parent, setVal); }
    else                         { widget = this->addUnknown(JSONvar, parent, setVal); }

    // store information for reset operations, data collection, and validation
    InputDataType *varData = new InputDataType;
    varData->name        = varName;
    varData->displayName = JSONvar["displayname"].toString();
    varData->type        = type;
    varData->defValue    = JSONvar["default"].toString();
    varData->widget      = widget;
    if (type == "choose") {
        varData->options = new QJsonObject(JSONvar["options"].toObject());
    }
    else {
        varData->options = NULL;
    }
    variableWidgets->insert(varName, varData);
}

bool PandPTabWidget::addVariable(QString varName, QJsonObject JSONvar, const QString &key, const QString &label, QString * setVal)
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
            this->addType(varName, type, JSONvar, parent, setVal);
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
    // enable the cancel button
    this->setButtonMode(CWE_BTN_CANCEL);
}

QMap<QString, QString> PandPTabWidget::collectParamData()
{
    QString val;
    QMap<QString, QString> currentParameters;

    // collect all parameter values
    foreach (const InputDataType *itm, variableWidgets->values())
    {
        // store information for reset operations, data collection, and validation
        QString varName  = itm->name;
        QString type     = itm->type;
        QString defValue = itm->defValue;
        QWidget *widget  = itm->widget;

        if (type == "std")         {
            val = tr("%1").arg(((QDoubleSpinBox *)widget)->value(), 0, 'g', 16);
        }
        else if (type == "bool")   {
            val = (((QCheckBox *)widget)->checkState() == Qt::Checked)?"true":"false" ;
        }
        else if (type == "file")   {
            val = ((QLineEdit *)widget)->text() ;
        }
        else if (type == "choose") {
            QString txt = ((QComboBox *)widget)->currentText() ;
            QJsonObject *options = itm->options;
            val = txt;
            foreach (QString key, options->keys()) {
                if (options->value(key) == txt) { val = key; }
            }
        }
        else {
            val = "";
        }

        // add to output
        currentParameters.insert(varName, val);
    }

    return currentParameters;
}

void PandPTabWidget::on_pbtn_cancel_clicked()
{
    // initiate job cancellation

    // enable the run button
    this->setButtonMode(CWE_BTN_RUN);
}

void PandPTabWidget::on_pbtn_results_clicked()
{
    // set run and rollback button active
    this->setButtonMode(CWE_BTN_RESULTS|CWE_BTN_ROLLBACK);

    // switch to the results tab
    emit switchToResultsTab();
}

void PandPTabWidget::on_pbtn_rollback_clicked()
{
    // reset the interface

    // set run button active
    this->setButtonMode(CWE_BTN_RUN);
}

void PandPTabWidget::on_groupTabSelected(int idx)
{
    this->setIndex(idx);

    // check for status of tab #idx

    // set button state accordingly
    this->setButtonMode(CWE_BTN_RUN);
}

QString PandPTabWidget::getStateText(StageState theState)
{
    if (theState == StageState::ERROR)
        return "*** ERROR ***";
    if (theState == StageState::FINISHED)
        return "Task Finished";
    if (theState == StageState::LOADING)
        return "Loading Data ...";
    if (theState == StageState::RUNNING)
        return "Task Running";
    if (theState == StageState::UNRUN)
        return "Not Yet Run";
    return "*** TOTAL ERROR ***";
}

void PandPTabWidget::setButtonMode(uint mode)
{
    bool btnState;

    btnState = (mode & CWE_BTN_RUN)?true:false;
    ui->pbtn_run->setEnabled(btnState);

    btnState = (mode & CWE_BTN_CANCEL)?true:false;
    ui->pbtn_cancel->setEnabled(btnState);

    btnState = (mode & CWE_BTN_RESULTS)?true:false;
    ui->pbtn_results->setEnabled(btnState);

    btnState = (mode & CWE_BTN_ROLLBACK)?true:false;
    ui->pbtn_rollback->setEnabled(btnState);

}
