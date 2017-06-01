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

#include "cfdtoken.h"

CFDtoken::CFDtoken()
{
    nullfiyContent();
}

CFDtoken::~CFDtoken()
{
    nullfiyContent();
}

CFDtokenType CFDtoken::getType()
{
    return myType;
}

int CFDtoken::getIntVal()
{
    return myInt;
}

double CFDtoken::getFloatVal()
{
    return myFloat;
}

QByteArray CFDtoken::getStringVal()
{
    if (myType == CFDtokenType::INT)
    {
        return QByteArray::number(myInt);
    }
    if (myType == CFDtokenType::FLOAT)
    {
        return QByteArray::number(myFloat);
    }
    return myString;
}

QLinkedList<CFDtoken *> CFDtoken::getChildList()
{
    return childList;
}

int CFDtoken::getChildSize()
{
    return childList.size();
}

CFDtoken * CFDtoken::getLargestChildArray()
{
    CFDtoken * ret = NULL;
    int refSize = -1;
    for (auto itr = childList.begin(); itr != childList.end(); itr++)
    {
        if ((*itr)->getType() == CFDtokenType::DATA_ARRAY)
        {
            if ((*itr)->getChildSize() > refSize)
            {
                ret = (*itr);
            }
        }
    }
    return ret;
}

CFDtoken * CFDtoken::getParent()
{
    return myParent;
}

QByteArray * CFDtoken::stripCFDcomments(QByteArray * rawInput)
{
    //Comments can be //
    /* or have the multiline format */

    while (rawInput->contains("/*"))
    {
        int startComment = rawInput->indexOf("/*");
        int endComment = rawInput->indexOf("*/", startComment);
        if (endComment != -1)
        {
            int commentLen = endComment - startComment + 2;
            rawInput->remove(startComment,commentLen);
        }
    }

    while (rawInput->contains("//"))
    {
        int startComment = rawInput->indexOf("//");
        int endComment = rawInput->indexOf("\n", startComment);
        if (endComment != -1)
        {
            int commentLen = endComment - startComment;
            rawInput->remove(startComment,commentLen);
        }
    }
    return rawInput;
}

CFDtoken * CFDtoken::lexifyString(QByteArray * rawInput)
{
    bool withinToken = false;

    rawInput = stripCFDcomments(rawInput);

    CFDtoken * ret = new CFDtoken();

    QByteArray oneToken;

    for (auto itr = rawInput->cbegin(); itr != rawInput->cend(); itr++)
    {
        char aLetter = (*itr);

        if (std::isspace(aLetter))
        {
            if (withinToken)
            {
                CFDtoken * tmp = new CFDtoken();
                tmp->setString(oneToken);
                ret->addChild(tmp);
                oneToken.clear();

                withinToken = false;
            }
        }
        else if ((aLetter == '(') || (aLetter == ')') || (aLetter == '{') || (aLetter == '}'))
        {
            if (withinToken)
            {
                CFDtoken * tmp = new CFDtoken();
                tmp->setString(oneToken);
                ret->addChild(tmp);
                oneToken.clear();

                withinToken = false;
            }
            CFDtoken * tmp = new CFDtoken();
            tmp->myType = CFDtokenType::SPECIAL_CHAR;
            tmp->specialChar = aLetter;
            ret->addChild(tmp);
        }
        else
        {
            if (!withinToken)
            {
                oneToken.clear();
                withinToken = true;
            }
            oneToken.append(aLetter);
        }
    }

    if (withinToken)
    {
        CFDtoken * tmp = new CFDtoken();
        tmp->setString(oneToken);
        ret->addChild(tmp);
    }

    return ret;
}

bool CFDtoken::parseTokenStream(CFDtoken * rootToken)
{
    //First, we perform {} and () matching to create data subnodes
    if (collapseParenTokens(rootToken) == false)
    {
        return false;
    }

    //Note: if we need to implement further parsing: Put it here

    return true;
}

QLinkedList<CFDtoken *>::iterator CFDtoken::insertParentToken(
        CFDtoken * rootToken,
        QLinkedList<CFDtoken *>::iterator start,
        QLinkedList<CFDtoken *>::iterator end)
{
    if ((*start)->getParent() != (*end)->getParent())
    {
        return rootToken->childList.end();
    }
    if ((*start)->getParent() != rootToken)
    {
        return rootToken->childList.end();
    }

    CFDtoken * tmpNode = (*start);
    start = rootToken->childList.erase(start);
    delete tmpNode;

    tmpNode = (*end);
    end = rootToken->childList.erase(end);
    delete tmpNode;

    QLinkedList<CFDtoken *>::iterator workPtr = start;
    CFDtoken * newTreeNode = new CFDtoken();
    newTreeNode->setParent(rootToken);

    while (workPtr != end)
    {
        newTreeNode->addChild(*workPtr);
        workPtr = rootToken->childList.erase(workPtr);
    }
    end = rootToken->childList.insert(end,newTreeNode);

    return end;
}

bool CFDtoken::collapseParenTokens(CFDtoken * rootToken)
{
    bool atFirstNode = true;
    bool foundOne = true;

    QLinkedList<CFDtoken *>::iterator startItr = rootToken->childList.begin();
    QLinkedList<CFDtoken *>::iterator endItr = startItr;

    while (foundOne)
    {
        foundOne = false;
        bool expectDataArray = false;

        while (startItr != rootToken->childList.end() &&
               !(*startItr)->isParenToken())
        {
            startItr++;
            atFirstNode = false; //Note: Can move this if it slows things too much
        }

        if (startItr == rootToken->childList.end())
        {
            return true;
        }

        endItr = startItr;
        char startParen = (*startItr)->specialChar;
        if ((startParen == '}') || (startParen == ')'))
        {
            return false;
        }
        int nowDepth = 1;

        endItr++;
        while ((endItr != rootToken->childList.end()) && (foundOne == false))
        {
            if ((*endItr)->isParenToken())
            {
                char endParen = (*endItr)->specialChar;

                if ((endParen == '{') || (endParen == '('))
                {
                    nowDepth++;
                }

                if ((endParen == '}') || (endParen == ')'))
                {
                    nowDepth--;
                    if (nowDepth < 0)
                    {
                        return false;
                    }

                    if (nowDepth == 0)
                    {
                        if ((startParen == '{') && (endParen == '}'))
                        {
                            foundOne = true;
                        }
                        else if ((startParen == '(') && (endParen == ')'))
                        {
                            foundOne = true;
                            expectDataArray = true;
                        }
                    }
                }
            }
            if (foundOne == false)
            {
                endItr++;
            }
        }

        if (endItr == rootToken->childList.end())
        {
            return false;
        }

        startItr = insertParentToken(rootToken,startItr,endItr);

        if (!collapseParenTokens(*startItr))
        {
            return false;
        }

        if (expectDataArray)
        {
            (*startItr)->setAsDataArray();

            if (!atFirstNode)
            {
                QLinkedList<CFDtoken *>::iterator beforeItr = startItr;
                beforeItr--;

                if ((*beforeItr)->getType() == CFDtokenType::INT)
                {
                    CFDtoken * expectedIntNode = (*beforeItr);
                    rootToken->childList.erase(beforeItr);
                    int numInArray = expectedIntNode->getIntVal();
                    delete expectedIntNode;

                    if (numInArray != (*startItr)->getChildSize())
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

bool CFDtoken::isParenToken()
{
    if (myType != CFDtokenType::SPECIAL_CHAR)
    {
        return false;
    }
    if (specialChar == '(') return true;
    if (specialChar == ')') return true;
    if (specialChar == '{') return true;
    if (specialChar == '}') return true;

    return false;
}

void CFDtoken::nullfiyContent()
{
    myType = CFDtokenType::INVALID;
    myString.clear();
    myInt = 0;
    myFloat = 0.0;

    while (!childList.isEmpty())
    {
        CFDtoken * aChild = childList.takeLast();
        delete aChild;
    }
}

void CFDtoken::setInt(int newVal)
{
    nullfiyContent();
    myType = CFDtokenType::INT;
    myInt = newVal;
}

void CFDtoken::setFloat(double newVal)
{
    nullfiyContent();
    myType = CFDtokenType::FLOAT;
    myFloat = newVal;
}

void CFDtoken::setString(QByteArray newVal)
{
    nullfiyContent();
    myType = CFDtokenType::STRING;
    myString = newVal;

    bool okCheck;
    myInt = myString.toInt(&okCheck);

    if (okCheck)
    {
        myType = CFDtokenType::INT;
        return;
    }

    myFloat = myString.toDouble(&okCheck);

    if (okCheck)
    {
        myType = CFDtokenType::FLOAT;
    }
}

void CFDtoken::addChild(CFDtoken * newChild)
{
    if ((myType != CFDtokenType::TREE_NODE) &&
            (myType != CFDtokenType::DATA_ARRAY))
    {
        nullfiyContent();
        myType = CFDtokenType::TREE_NODE;
    }
    newChild->setParent(this);
    childList.append(newChild);
}

void CFDtoken::setAsDataArray()
{
    if ((myType != CFDtokenType::TREE_NODE) &&
            (myType != CFDtokenType::DATA_ARRAY))
    {
        nullfiyContent();
    }
    myType = CFDtokenType::DATA_ARRAY;
}

void CFDtoken::setParent(CFDtoken * newParent)
{
    myParent = newParent;
}
