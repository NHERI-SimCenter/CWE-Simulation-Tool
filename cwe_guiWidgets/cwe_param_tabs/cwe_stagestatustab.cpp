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

#include "cwe_stagestatustab.h"
#include "ui_cwe_stagestatustab.h"

CWE_StageStatusTab::CWE_StageStatusTab(QString theStageKey, QString stageName, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CWE_StageStatusTab)
{
    ui->setupUi(this);
    stageKey = theStageKey;
    ui->mainLabel->setText(stageName);

    this->setStatus("INIT");
    this->setButtonAppearance(false, false);
}

CWE_StageStatusTab::~CWE_StageStatusTab()
{
    delete ui;
}

void CWE_StageStatusTab::setStatus(const QString str)
{
    stageStatus = str;
    ui->statusLabel->setText(str);
}

QString CWE_StageStatusTab::getStageKey()
{
    return stageKey;
}

QString CWE_StageStatusTab::status()
{
    return stageStatus;
}

bool CWE_StageStatusTab::tabIsActive()
{
    return tab_active;
}

void CWE_StageStatusTab::setActive(bool b)
{
    if (!b)
    {
        this->setInActive();
        return;
    }
    setButtonAppearance(tab_pressed, true);
}

void CWE_StageStatusTab::setInActive(bool b)
{
    if (!b)
    {
        this->setActive();
        return;
    }
    setButtonAppearance(tab_pressed, false);
}

void CWE_StageStatusTab::mousePressEvent(QMouseEvent *event)
{
    //TODO: Test mouse release if mouse dragged off tab
    if (event->button() == Qt::LeftButton)
    {
        setButtonAppearance(true, tab_active);
        emit btn_pressed(this);
    }
}

void CWE_StageStatusTab::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        setButtonAppearance(false, tab_active);
        emit btn_released(this);
    }
}

void CWE_StageStatusTab::setButtonAppearance(bool tabPressed, bool tabActive)
{
    tab_pressed = tabPressed;
    tab_active = tabActive;
    if (tab_pressed)
    {
        if (tab_active)
        {
            this->setStyleSheet("QFrame {background: #B0BEC5; border-color: #808080; border-width: 1.5px; border-radius: 3px; border-style: inset;} QLabel {border-style: none}");
        }
        else
        {
            this->setStyleSheet("QFrame {background: #B0BEC5; border-color: #808080; border-width: 2px; border-radius: 3px; border-style: onset;} QLabel {border-style: none}");
        }
    }
    else
    {
        if (tab_active)
        {
            this->setStyleSheet("QFrame {background: #64B5F6; border-color: #808080; border-width: 1.5px; border-radius: 3px; border-style: inset;} QLabel {border-style: none}");
        }
        else
        {
            this->setStyleSheet("QFrame {background: #C0C0C8; border-color: #808080; border-width: 2px; border-radius: 3px; border-style: onset;} QLabel {border-style: none}");
        }
    }
}
