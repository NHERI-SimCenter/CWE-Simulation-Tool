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

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QJsonObject>
#include <QBoxLayout>
#include <QKeyValueIterator>

#include "../AgaveExplorer/remoteFileOps/filenoderef.h"
#include "CFDanalysis/CFDanalysisType.h"

class FileTreeNode;

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

enum class SimCenterViewState  { visible,
                                 editable,
                                 hidden };

struct VARIABLE_TYPE {
    QString displayName;
    QString type;
    QString defaultValue;
    QString unit;
    QString precision;
    QString sign;
    QMap<QString, QString> options;
};

class SCtrMasterDataWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SCtrMasterDataWidget(QWidget *parent = 0);
    ~SCtrMasterDataWidget();
    SimCenterViewState viewState();
    virtual void setViewState(SimCenterViewState);

    void setDataType(VARIABLE_TYPE &);
    VARIABLE_TYPE getTypeInfo();

    virtual void updateValue(QString) = 0;
    virtual QString value() = 0;

    virtual bool valueChanged() = 0;

protected:
    //QLineEdit *theValue = NULL;
    //QCheckBox *theCheckBox = NULL;
    //QComboBox *theComboBox = NULL;

    //QLabel  *label_varName = NULL;
    //QLabel  *label_unit = NULL;

    VARIABLE_TYPE m_obj;

private:
    virtual void initUI() = 0;
    virtual setComponetsEnabled(bool newSetting) = 0;

    QString currentValue;

    SimCenterViewState m_ViewState;

    QValidator *m_validator = NULL;
    SimCenterDataType m_dataType;
};

#endif // SCTRDATAWIDGET_H
