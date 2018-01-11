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

#include "SimCenter_widgets/sctrmasterdatawidget.h"
#include "SimCenter_widgets/sctrvalidators.h"

SCtrMasterDataWidget::SCtrMasterDataWidget(QWidget *parent) :
    QFrame(parent)
{
    this->setViewState(SimCenterViewState::visible);

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
    if (label_unit == NULL) {
        label_unit = new QLabel(this);
    }
    if (label_varName == NULL) {
        label_varName = new QLabel(this);
    }
    QBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label_varName, 3);
    layout->addWidget(label_unit, 1);
    this->setLayout(layout);
}

SimCenterViewState SCtrMasterDataWidget::ViewState()
{
    return m_ViewState;
}

void SCtrMasterDataWidget::setViewState(SimCenterViewState state)
{
    switch (state) {
    case SimCenterViewState::visible:
        if (theValue    != NULL) theValue->setEnabled(false);
        if (theComboBox != NULL) theComboBox->setEnabled(false);
        if (theCheckBox != NULL) theCheckBox->setEnabled(false);
        this->show();
        m_ViewState = SimCenterViewState::editable;
        break;
    case SimCenterViewState::hidden:
        if (theValue    != NULL) theValue->setEnabled(true);
        if (theComboBox != NULL) theComboBox->setEnabled(true);
        if (theCheckBox != NULL) theCheckBox->setEnabled(true);
        this->hide();
        m_ViewState = SimCenterViewState::hidden;
        break;
    case SimCenterViewState::editable:
    default:
        if (theValue    != NULL) theValue->setEnabled(true);
        if (theComboBox != NULL) theComboBox->setEnabled(true);
        if (theCheckBox != NULL) theCheckBox->setEnabled(true);
        this->show();
        m_ViewState = SimCenterViewState::visible;
        break;
    }
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

QString SCtrMasterDataWidget::toString()
{
    return QString("");
}

/* ********** helper functions ********** */

void SCtrMasterDataWidget::setVariableName(QString s)
{
    if (label_varName != NULL) label_varName->setText(s);
}

void SCtrMasterDataWidget::setUnit(QString u)
{
    if (label_unit != NULL) label_unit->setText(u);
}

/* ********** SLOTS ********** */
void SCtrMasterDataWidget::on_theValue_editingFinished()
{

}

void SCtrMasterDataWidget::newFileSelected(FileTreeNode *)
{

}
