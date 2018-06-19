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

#include "sctrbooldatawidget.h"

#include "cwe_globals.h"

SCtrBoolDataWidget::SCtrBoolDataWidget(QWidget *parent):
    SCtrMasterDataWidget(parent)
{

}

void SCtrBoolDataWidget::setDataType(VARIABLE_TYPE &obj)
{
    // set up the UI for the widget
    this->initUI();

    m_obj = obj;

    QHBoxLayout *layout = (QHBoxLayout *)this->layout();
    layout->setMargin(0);

    theCheckBox = new QCheckBox(this);
    layout->insertWidget(1, theCheckBox, 4);

    if (label_unit != NULL) {
        label_unit->setText(m_obj.unit);
    }
    if (label_varName != NULL) {
        label_varName->setText(m_obj.displayName);
    }

    /* set default */
    if (m_obj.defaultValue.toLower() == "true")
    {
        theCheckBox->setChecked(true);
    }
    else
    {
        theCheckBox->setChecked(false);
    }
}

bool SCtrBoolDataWidget::toBool()
{
    bool checked = theCheckBox->checkState()==Qt::Checked?true:false;
    return checked;
}

QString SCtrBoolDataWidget::toString()
{
    QString checked = theCheckBox->checkState()==Qt::Checked?QString("true"):QString("false");
    return checked;
}

double SCtrBoolDataWidget::toDouble()
{
    double checked = theCheckBox->checkState()==Qt::Checked?1.0:0.0;
    return checked;
}

void SCtrBoolDataWidget::setChecked()
{
    theCheckBox->setChecked(true);
}

void SCtrBoolDataWidget::setUnchecked()
{
    theCheckBox->setChecked(false);
}

void SCtrBoolDataWidget::updateValue(QString s)
{
    /* check if new information is an appropriate type */
    if (s.toLower() == "true")
        { setChecked(); }
    else if (s.toLower() == "false")
        {  setUnchecked(); }
    else
    {
        /* add an error message */
        cwe_globals::displayPopup(QString("Boolean variable %1 cannot be set to \'%2\'.\nVariable ignored.").arg(m_obj.displayName).arg(s), "Warning");
        return;
    }
}

