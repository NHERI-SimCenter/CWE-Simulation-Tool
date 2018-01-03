#ifndef SCTRDATAWIDGET_H
#define SCTRDATAWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QValidator>
#include <QObject>

#include "cwe_guiWidgets/cwe_defines.h"
#include <SimCenter_widgets/sctrstates.h>

class SCtrMasterDataWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SCtrMasterDataWidget(QWidget *parent = 0);
    ~SCtrMasterDataWidget();
    SimCenterViewState ViewState();
    void setViewState(SimCenterViewState);
    virtual void setData(QJsonObject &);
    virtual void initUI();
    virtual void setValue(QString);
    virtual void setValue(float);
    virtual void setValue(int);
    virtual void setValue(bool);
    virtual QString toString();
    virtual void updateValue(QString);
    QString value() {return this->toString();}

protected:
    QFrame * addStd(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addBool(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addFile(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addChoice(QJsonObject, QWidget *parent, QString *setVal = NULL);
    QFrame * addVector3D(QJsonObject JSONvar, QWidget *parent, QString *setVal = NULL );
    QFrame * addVector2D(QJsonObject JSONvar, QWidget *parent, QString *setVal = NULL );
    QFrame * addTensor3D(QJsonObject JSONvar, QWidget *parent, QString *setVal = NULL );
    QFrame * addTensor2D(QJsonObject JSONvar, QWidget *parent, QString *setVal = NULL );
    QFrame * addUnknown(QJsonObject, QWidget *parent, QString *setVal = NULL );
    void addType(const QString &, const QString &, QJsonObject, QWidget *parent , QString * setVal);

private slots:
    void on_theValue_editingFinished();

protected:
    QLineEdit *theValue;
    QCheckBox *theCheckBox;
    QComboBox *theComboBox;

    QLabel  *label_varName = NULL;
    QLabel  *label_unit = NULL;

    QJsonObject m_obj;

private:
    void setVariableName(QString s);
    void setUnit(QString u);

    void showLineEdit();
    void showCheckBox();
    void showComboBox();

    SimCenterViewState m_ViewState;

    QValidator *m_validator = NULL;
    SimCenterDataType m_dataType;
};

#endif // SCTRDATAWIDGET_H
