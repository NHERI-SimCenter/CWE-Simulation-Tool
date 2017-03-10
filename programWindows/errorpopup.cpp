/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
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
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "errorpopup.h"
#include "ui_errorpopup.h"

ErrorPopup::ErrorPopup(VWTerrorType errNum) :
    QDialog(0),
    ui(new Ui::ErrorPopup)
{
    ui->setupUi(this);

    errorVal = errNum;
    setErrorLabel(getErrorText(errNum));

    //TODO: not sure if calling exec during constructor is a good idea
    this->exec();
}

ErrorPopup::ErrorPopup(QString errorText) :
    QDialog(0),
    ui(new Ui::ErrorPopup)
{
    ui->setupUi(this);

    errorVal = VWTerrorType::CUSTOM_ERROR;
    setErrorLabel(errorText);

    //TODO: not sure if calling exec during constructor is a good idea
    this->exec();
}

void ErrorPopup::setErrorLabel(QString errorText)
{
    QLabel * errorTextElement = this->findChild<QLabel *>("errorText");

    QString realErrorText = errorText;
    realErrorText.prepend(": ");
    realErrorText.prepend(QString::number((unsigned int)errNum));
    realErrorText.prepend("ERROR ");

    errorTextElement->setText(realErrorText);
    qDebug("%s",realErrorText.toStdString().c_str());
}

void ErrorPopup::closeByError()
{
    qApp->exit((unsigned int)errorVal);
}

ErrorPopup::~ErrorPopup()
{
    delete ui;
}

QString ErrorPopup::getErrorText(VWTerrorType errNum)
{
    switch(errNum)
    {
        case VWTerrorType::ERR_NO_DEF: return "FATAL ERROR";
        case VWTerrorType::ERR_NOT_IMPLEMENTED: return "Feature Not Yet Implemented";
        case VWTerrorType::ERR_ACCESS_LOST: return "Access Connection to Design Safe Lost";
        case VWTerrorType::ERR_WINDOW_SYSTEM: return "Window System Lost its windows";
        case VWTerrorType::ERR_AUTH_BLANK: return "Authorization for request not available";
        default: return "FATAL ERROR";
    }
    return "FATAL ERROR";
}
