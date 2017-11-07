#ifndef SCTRDATAWIDGET_H
#define SCTRDATAWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>
#include <QValidator>

#include "cwe_guiWidgets/cwe_defines.h"

enum class SimCenterViewState { visible,
                                editable,
                                hidden };

enum class SimCenterDataType { integer,
                               floatingpoint,
                               boolean,
                               string,
                               selection,
                               file,
                               tensor2D,
                               tensor3D,
                               vector2D,
                               vector3D,
                               unknown};


class SCtrMasterDataWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SCtrMasterDataWidget(QWidget *parent = 0);
    ~SCtrMasterDataWidget();
    SimCenterViewState ViewState();
    void setViewState(SimCenterViewState);
    void setData(QJsonObject &);
    virtual void initUI();
    virtual void setValue(QString);
    virtual void setValue(float);
    virtual void setValue(int);
    virtual void setValue(bool);
    virtual QString Value();

protected:
    QFrame * addStd(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addBool(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addFile(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addChoice(QJsonObject, QWidget *parent, QString *setVal = NULL);
    QFrame * addVector3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addVector2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addTensor3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addTensor2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QFrame * addUnknown(QJsonObject, QWidget *parent, QString *setVal = NULL );
    void addType(const QString &, const QString &, QJsonObject, QWidget *parent , QString * setVal);

private slots:
    void on_theValue_editingFinished();

private:
    void showLineEdit();
    void showCheckBox();
    void showComboBox();

    QLineEdit *theValue;
    QCheckBox *theCheckBox;
    QComboBox *theComboBox;
    QLabel *label_varName;
    QLabel *label_unit;

    SimCenterViewState m_ViewState;

    QJsonObject m_obj;
    QValidator *m_validator = NULL;
    SimCenterDataType m_dataType;

    QMap<QString, InputDataType *> *variableWidgets;
};

#endif // SCTRDATAWIDGET_H
