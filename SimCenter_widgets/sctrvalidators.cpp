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

#include "sctrvalidators.h"

SCtrNoValidator::SCtrNoValidator(QObject *parent):
    QValidator(parent)
{

}

QValidator::State SCtrNoValidator::validate(QString &, int &) const
{
    return QValidator::Acceptable;
}

SCtrBoolValidator::SCtrBoolValidator(QObject *parent):
    QValidator(parent)
{

}

QValidator::State SCtrBoolValidator::validate(QString &input, int &) const
{
    if (input.toLower() == "f")     return QValidator::Acceptable;
    if (input.toLower() == "fa")    return QValidator::Acceptable;
    if (input.toLower() == "fal")   return QValidator::Acceptable;
    if (input.toLower() == "fals")  return QValidator::Acceptable;
    if (input.toLower() == "false") return QValidator::Acceptable;
    if (input.toLower() == "t")     return QValidator::Acceptable;
    if (input.toLower() == "tr")    return QValidator::Acceptable;
    if (input.toLower() == "tru")   return QValidator::Acceptable;
    if (input.toLower() == "true")  return QValidator::Acceptable;

    return QValidator::Invalid;
}
