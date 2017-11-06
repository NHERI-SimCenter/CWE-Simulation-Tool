#ifndef SCTRDATAWIDGET_H
#define SCTRDATAWIDGET_H

#include <QWidget>
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

namespace Ui {
class SCtrDataWidget;
}

class SCtrDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SCtrDataWidget(QWidget *parent = 0);
    ~SCtrDataWidget();
    SimCenterViewState ViewState();
    void setViewState(SimCenterViewState);
    void setData(QJsonObject &);
    void setValue(QString);
    void setValue(float);
    void setValue(int);
    void setValue(bool);
    QString Value();

protected:
    QWidget * addStd(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addBool(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addFile(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addChoice(QJsonObject, QWidget *parent, QString *setVal = NULL);
    QWidget * addVector3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addVector2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addTensor3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addTensor2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addUnknown(QJsonObject, QWidget *parent, QString *setVal = NULL );
    void addType(const QString &, const QString &, QJsonObject, QWidget *parent , QString * setVal);

private slots:
    void on_theValue_editingFinished();

private:
    void showLineEdit();
    void showCheckBox();
    void showComboBox();

    Ui::SCtrDataWidget *ui;
    SimCenterViewState m_ViewState;

    QJsonObject m_obj;
    QValidator *m_validator = NULL;
    SimCenterDataType m_dataType;

    QMap<QString, InputDataType *> *variableWidgets;
};

#endif // SCTRDATAWIDGET_H
