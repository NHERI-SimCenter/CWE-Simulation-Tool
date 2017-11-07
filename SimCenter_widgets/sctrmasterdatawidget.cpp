#include "sctrmasterdatawidget.h"

#include "cwe_guiWidgets/cwe_defines.h"

#include <QFrame>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGridLayout>

#include "SimCenter_widgets/sctrvalidators.h"

SCtrMasterDataWidget::SCtrMasterDataWidget(QWidget *parent) :
    QFrame(parent)
{
    // set up the UI for the widget

    this->setViewState(SimCenterViewState::visible);

    variableWidgets = new QMap<QString, InputDataType *>();

    m_obj = QJsonObject();
    m_obj.insert("type","unknown");
    m_obj.insert("displayname","not specified");
    m_obj.insert("varname","UNKNOWN");
}

SCtrMasterDataWidget::~SCtrMasterDataWidget()
{
    if (m_validator != NULL) delete m_validator;
    m_validator = NULL;
}


void SCtrMasterDataWidget::initUI()
{

}

SimCenterViewState SCtrMasterDataWidget::ViewState()
{
    return m_ViewState;
}

void SCtrMasterDataWidget::setViewState(SimCenterViewState state)
{
    switch (state) {
    case SimCenterViewState::visible:
        theValue->setEnabled(false);
        theCheckBox->setEnabled(false);
        theComboBox->setEnabled(false);
        this->show();
        m_ViewState = state;
        break;
    case SimCenterViewState::editable:
        theValue->setEnabled(true);
        theCheckBox->setEnabled(true);
        theComboBox->setEnabled(true);
        this->show();
        m_ViewState = state;
        break;
    case SimCenterViewState::hidden:
        theValue->setEnabled(false);
        theCheckBox->setEnabled(false);
        theComboBox->setEnabled(false);
        this->hide();
        m_ViewState = state;
        break;
    default:
        m_ViewState = SimCenterViewState::visible;
    }
}

void SCtrMasterDataWidget::setData(QJsonObject &obj)
{
    QString setVal = "";
    QJsonObject combo_options;
    QStandardItemModel *theModel = NULL;

    m_obj = obj;
    if (m_validator != NULL) delete m_validator;
    m_validator = NULL;

    // switch the type to a member of SimCenterDataType so we can use switch statement
    QString type           = obj["type"].toString();
    m_dataType = SimCenterDataType::unknown;
    if (type.toLower() == "file")     m_dataType = SimCenterDataType::file;
    if (type.toLower() == "std")      m_dataType = SimCenterDataType::floatingpoint;
    if (type.toLower() == "bool")     m_dataType = SimCenterDataType::boolean;
    if (type.toLower() == "string")   m_dataType = SimCenterDataType::string;
    if (type.toLower() == "tensor2D") m_dataType = SimCenterDataType::tensor2D;
    if (type.toLower() == "tensor3D") m_dataType = SimCenterDataType::tensor3D;
    if (type.toLower() == "vector2D") m_dataType = SimCenterDataType::vector2D;
    if (type.toLower() == "vector3D") m_dataType = SimCenterDataType::vector3D;
    if (type.toLower() == "int")      m_dataType = SimCenterDataType::integer;
    if (type.toLower() == "choose")   m_dataType = SimCenterDataType::selection;

    QVariant defaultOption = obj["default"].toVariant();

    QString data = obj["default"].toString();
    QString sign = obj["sign"].toString();
    int precision = 0;

    switch (m_dataType) {
    case SimCenterDataType::integer:
        this->showLineEdit();

        m_validator = new QIntValidator(theValue);

        if      (sign.contains("+0") || sign.contains("0+") ) { ((QIntValidator *)m_validator)->setBottom(0); }
        else if (sign.contains("+"))                          { ((QIntValidator *)m_validator)->setBottom(1); }
        else if (sign.contains("-0") || sign.contains("0-") ) { ((QIntValidator *)m_validator)->setTop(0);    }
        else if (sign.contains("-"))                          { ((QIntValidator *)m_validator)->setTop(-1);   }

        theValue->setValidator(m_validator);
        break;

    case SimCenterDataType::floatingpoint:
        this->showLineEdit();

        m_validator = new QDoubleValidator(theValue);

        // QJson fails to convert "1" to int, thus: QString::toInt( QJson::toString() )
        precision = obj["precision"].toString().toInt();
        if (precision > 0) ((QDoubleValidator *)m_validator)->setDecimals(precision);

        if      (sign.contains("+")) { ((QDoubleValidator *)m_validator)->setBottom(0.0); }
        else if (sign.contains("-")) { ((QDoubleValidator *)m_validator)->setTop(0.0);    }

        theValue->setValidator(m_validator);
        break;

    case SimCenterDataType::string:
        this->showLineEdit();
        break;

    case SimCenterDataType::boolean:
        this->showCheckBox();
        break;

    case SimCenterDataType::file:
        this->showLineEdit();
        break;

    case SimCenterDataType::vector2D:
        this->showLineEdit();
        break;

    case SimCenterDataType::vector3D:
        this->showLineEdit();
        break;

    case SimCenterDataType::tensor2D:
        this->showLineEdit();
        break;

    case SimCenterDataType::tensor3D:
        this->showLineEdit();
        break;

    case SimCenterDataType::selection:
        this->showComboBox();

        combo_options = obj["options"].toObject();

        theModel = new QStandardItemModel();
        foreach (const QString &theKey, combo_options.keys())
        {
            //QStandardItem *itm = new QStandardItem(theKey);
            QStandardItem *itm = new QStandardItem(combo_options[theKey].toString());
            theModel->appendRow(itm);
        }
        theComboBox->setModel(theModel);

        setVal = obj["default"].toString();
        if (combo_options.contains(setVal))
        {
            theComboBox->setCurrentText(combo_options[setVal].toString());
        }

        break;

    case SimCenterDataType::unknown:
    default:
        this->showLineEdit();
        break;
    }

    theValue->setText(data);

    QString displayname    = obj["displayname"].toString();
    label_varName->setText(displayname);

    QString unit           = obj["unit"].toString();
    label_unit->setText(unit);
}

void SCtrMasterDataWidget::setValue(QString s)
{
    theValue->setText(s);
}

void SCtrMasterDataWidget::setValue(float v)
{
    QString s = QString("%f").arg(v);
    theValue->setText(s);
}

void SCtrMasterDataWidget::setValue(int i)
{
    QString s = QString("%d").arg(i);
    theValue->setText(s);
}

void SCtrMasterDataWidget::setValue(bool b)
{
    QString s = b?"true":"false";
    theValue->setText(s);
}

QString SCtrMasterDataWidget::Value()
{
    return theValue->text();
}




/* ********** various input data types ********** */

QFrame * SCtrMasterDataWidget::addStd(QJsonObject JSONvar, QWidget *parent, QString *setVal)
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

    return this;
}

QFrame * SCtrMasterDataWidget::addBool(QJsonObject JSONvar, QWidget *parent, QString * setVal)
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

    return this;
}

QFrame * SCtrMasterDataWidget::addFile(QJsonObject JSONvar, QWidget *parent, QString * setVal)
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

    return this;
}

QFrame * SCtrMasterDataWidget::addChoice(QJsonObject JSONvar, QWidget *parent, QString * setVal)
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

    return this;
}

QFrame * SCtrMasterDataWidget::addVector3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QFrame * SCtrMasterDataWidget::addVector2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QFrame * SCtrMasterDataWidget::addTensor3D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QFrame * SCtrMasterDataWidget::addTensor2D(QJsonObject JSONvar, QWidget *parent, QString *setVal )
{
    return NULL;
}

QFrame * SCtrMasterDataWidget::addUnknown(QJsonObject JSONvar, QWidget *parent, QString *setVal)
{
    QLabel *theName = new QLabel(parent);
    QString displayname = JSONvar["displayname"].toString();
    theName->setText(displayname);

    QGridLayout *layout = (QGridLayout*)(parent->layout());
    int row = layout->rowCount();
    layout->addWidget(theName,row,0);

    return NULL;
}

void SCtrMasterDataWidget::addType(const QString &varName, const QString &type, QJsonObject JSONvar, QWidget *parent, QString *setVal)
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



/* ********** helper functions ********** */
void SCtrMasterDataWidget::showLineEdit()
{
    theValue->show();
    theCheckBox->hide();
    theComboBox->hide();
}

void SCtrMasterDataWidget::showCheckBox()
{
    theValue->hide();
    theCheckBox->show();
    theComboBox->hide();
}

void SCtrMasterDataWidget::showComboBox()
{
    theValue->hide();
    theCheckBox->hide();
    theComboBox->show();
}


/* ********** SLOTS ********** */
void SCtrMasterDataWidget::on_theValue_editingFinished()
{

}
