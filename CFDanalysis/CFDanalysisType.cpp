/*********************************************************************************
**
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
// Renamed, modifed by Peter Sempolinski

#include "CFDanalysisType.h"
#include "SimCenter_widgets/sctrmasterdatawidget.h"

#include "../AgaveExplorer/ae_globals.h"

CFDanalysisType::CFDanalysisType(QString configFile)
{
    QFile inFile(configFile);
    inFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray val = inFile.readAll();
    inFile.close();

    myConfiguration = QJsonDocument::fromJson(val);

    QJsonObject obj = myConfiguration.object();
    QString theIcon = obj["icon"].toString();

    //Find the template icon
    QString theIconPath;
    if (theIcon == "")
    {
        theIconPath = ":/buttons/images/defaultCaseImage.png";
    }
    else
    {
        theIconPath = ":/buttons/images/" + theIcon;
    }
    myIcon = QIcon(theIconPath);

    //Initialize the cached stage list
    QStringList stageList = obj["stages"].toObject().keys();;

    for (QJsonValue aStage : obj["sequence"].toArray())
    {
        QString stageID = aStage.toString();
        if (stageID.isEmpty()) continue;
        if (!stageList.contains(stageID))
        {
            qCDebug(agaveAppLayer, "Config Parse Error: Sequence list and stage list do not match");
            continue;
        }

        cachedOrderedStageList.append(stageID);
    }

    if (cachedOrderedStageList.length() != stageList.length())
    {
        qCWarning(agaveAppLayer, "Config Parse Error: Sequence list and stage list do not match");
    }
}

QJsonDocument * CFDanalysisType::getRawConfig()
{
    return &myConfiguration;
}

QString CFDanalysisType::getInternalName()
{
    QJsonObject obj = myConfiguration.object();
    return obj["internalName"].toString();
}

QString CFDanalysisType::getDisplayName()
{
    QJsonObject obj = myConfiguration.object();
    return obj["displayName"].toString();
}

QString CFDanalysisType::getDescription()
{
    QJsonObject obj = myConfiguration.object();
    return obj["description"].toString();
}

QString CFDanalysisType::getIconName()
{
    QJsonObject obj = myConfiguration.object();
    return obj["icon"].toString();
}

QStringList CFDanalysisType::getStageGroups(QString stage)
{
    QStringList list;

    QJsonObject obj = myConfiguration.object();
    QJsonArray groups = obj["stages"].toObject().value(stage).toObject().value("groups").toArray();

    foreach (QJsonValue item, groups)
    {
        list.append(item.toString());
    }

    return list;
}

QList<RESULTS_STYLE> CFDanalysisType::getStageResults(QString stage)
{
    QList<RESULTS_STYLE> list;

    QJsonObject obj = myConfiguration.object();
    QJsonArray results = obj["stages"].toObject().value(stage).toObject().value("results").toArray();

    foreach (QJsonValue val, results)
    {
        QJsonObject item = val.toObject();

        RESULTS_STYLE res;

        if (item.contains("displayName")) { res.displayName = item.value("displayName").toString(); }
        if (item.contains("type")) { res.type = item.value("type").toString(); }
        if (item.contains("file")) { res.file = item.value("file").toString(); }
        if (item.contains("values")) { res.values = item.value("values").toString(); }
        res.stage = stage;

        list.append(res);
    }

    return list;
}

QStringList CFDanalysisType::getVarGroup(QString group)
{
    QStringList list;

    QJsonObject obj = myConfiguration.object();
    QJsonArray vars = obj["groups"].toObject().value(group).toObject().value("vars").toArray();

    foreach (QJsonValue val, vars)
    {
        QString str = val.toString();
        if (!str.isEmpty())
        {
            list.append( str);
        }
    }

    return list;
}

VARIABLE_TYPE CFDanalysisType::getVariableInfo(QString name)
{
    VARIABLE_TYPE res;

    res.unit = "";
    res.precision = "";
    res.sign = "";
    res.options.clear();

    QJsonObject obj = myConfiguration.object();
    QJsonObject vals = obj["vars"].toObject();

    if (vals.contains(name))
    {
        QJsonObject item = vals.value(name).toObject();

        if (item.contains("displayName")) { res.displayName = item.value("displayName").toString(); }
        if (item.contains("default"))     { res.defaultValue = item.value("default").toString(); }
        if (item.contains("unit"))        { res.unit = item.value("unit").toString(); }
        if (item.contains("precision"))   { res.precision = item.value("precision").toString(); }
        if (item.contains("sign"))        { res.sign = item.value("sign").toString(); }
        if (item.contains("options"))
        {
            foreach (QString key, item["options"].toObject().keys())
            {
                res.options.insert(key, item["options"].toObject().value(key).toString());
            }
        }
        if (item.contains("type"))
        {
            QString typeName = item.value("type").toString();

            if (typeName == "std")
            {
                res.type = SimCenterDataType::floatingpoint;
            }
            else if (typeName == "file")
            {
                res.type = SimCenterDataType::file;
            }
            else if (typeName == "choose")
            {
                res.type = SimCenterDataType::selection;
            }
            else if (typeName == "bool")
            {
                res.type = SimCenterDataType::boolean;
            }
            else
            {
                res.type = SimCenterDataType::unknown;
            }
        }
    }
    else
    {
        res.displayName   = "none";
        res.type          = SimCenterDataType::unknown;
        res.defaultValue  = QString("missing definition for variable '%1'").arg(name);
    }

    return res;
}

QString CFDanalysisType::getStageApp(QString stageID)
{
    QJsonObject obj = myConfiguration.object();
    QString tmpStr = obj["stages"].toObject().value(stageID).toObject().value("app").toString();
    if (tmpStr.isEmpty())
    {
        return "cwe-serial";
    }
    return tmpStr;
}

QString CFDanalysisType::getExtraInput(QString stageID)
{
    QJsonObject obj = myConfiguration.object();
    QString tmpStr = obj["stages"].toObject().value(stageID).toObject().value("app_input").toString();
    return tmpStr;
}

QStringList CFDanalysisType::getStageIds()
{
    return cachedOrderedStageList;
}

QString CFDanalysisType::translateStageId(QString stageId)
{
    QJsonObject obj = myConfiguration.object();
    QJsonObject stageList = obj["stages"].toObject();

    QString aStage = stageList[stageId].toObject()["displayName"].toString();
    if (aStage.isEmpty())
    {
        return "NULL";
    }
    return aStage;
}

QString CFDanalysisType::translateGroupId(QString groupId)
{
    QJsonObject obj = myConfiguration.object();
    QJsonObject stageList = obj["groups"].toObject();

    QString aGroup = stageList[groupId].toObject()["displayName"].toString();
    if (aGroup.isEmpty())
    {
        return "NULL";
    }
    return aGroup;
}

QIcon * CFDanalysisType::getIcon()
{
    return &myIcon;
}

bool CFDanalysisType::isDebugOnly()
{
    QJsonObject obj = myConfiguration.object();
    if (!obj.contains("debugOnly"))
    {
        return false;
    }
    if (!obj["debugOnly"].isBool())
    {
        return false;
    }
    if (obj["debugOnly"].toBool() == true)
    {
        return true;
    }
    return false;
}

bool CFDanalysisType::isDisabled()
{
    QJsonObject obj = myConfiguration.object();
    if (!obj.contains("disable"))
    {
        return false;
    }
    if (!obj["disable"].isBool())
    {
        return false;
    }
    if (obj["disable"].toBool() == true)
    {
        return true;
    }
    return false;
}


