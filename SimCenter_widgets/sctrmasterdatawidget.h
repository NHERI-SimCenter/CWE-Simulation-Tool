/*********************************************************************************
**
** Copyright (c) 2018 The University of Notre Dame
** Copyright (c) 2018 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:

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
    virtual void setData(QJsonObject &) = 0;
    virtual void initUI();
    virtual void setValue(QString);
    virtual void setValue(float);
    virtual void setValue(int);
    virtual void setValue(bool);
    virtual QString toString();
    virtual void updateValue(QString);
    QString value() {return this->toString();}

protected:
    /*
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
    */

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

    SimCenterViewState m_ViewState;

    QValidator *m_validator = NULL;
    SimCenterDataType m_dataType;
};

#endif // SCTRDATAWIDGET_H
