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

#ifndef CFDTOKEN_H
#define CFDTOKEN_H

#include <QByteArray>
#include <QLinkedList>

//Note: this is not a complete parser,
//but has the framework if anyone wants to complete it later

//TODO Note: Int and float types might be vulnerable to overflow,
//check the setString function if you wish to change this

enum class CFDtokenType
{
    STRING,
    INVALID,
    TREE_NODE,
    INT,
    FLOAT,
    DATA_ARRAY,
    SPECIAL_CHAR
};

class CFDtoken
{
public:
    CFDtoken();
    ~CFDtoken();

    CFDtokenType getType();
    int getIntVal();
    double getFloatVal();
    QByteArray getStringVal();
    QLinkedList<CFDtoken *> getChildList();
    int getChildSize();
    CFDtoken * getParent();

    CFDtoken * getLargestChildArray();

    static QByteArray * stripCFDcomments(QByteArray * rawInput);
    static CFDtoken * lexifyString(QByteArray * rawInput);
    static bool parseTokenStream(CFDtoken * rootToken);

private:
    void nullfiyContent();

    void setInt(int newVal);
    void setFloat(double newVal);
    void setString(QByteArray newVal);
    void addChild(CFDtoken * newChild);
    void setAsDataArray();
    void setParent(CFDtoken * newParent);

    static bool collapseParenTokens(CFDtoken * rootToken);
    static QLinkedList<CFDtoken *>::iterator insertParentToken(
            CFDtoken * rootToken,
            QLinkedList<CFDtoken *>::iterator start,
            QLinkedList<CFDtoken *>::iterator end);

    bool isParenToken();

    CFDtokenType myType;

    CFDtoken * myParent = NULL;
    QLinkedList<CFDtoken *> childList;
    QByteArray myString;
    int myInt;
    double myFloat;

    char specialChar;
};

#endif // CFDTOKEN_H
