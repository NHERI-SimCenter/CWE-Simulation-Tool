#ifndef PANDPTABWIDGET_H
#define PANDPTABWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>
#include <QTabWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QLayout>

#include "cwe_defines.h"

class CWE_WithStatusButton;
enum class StageState;

namespace Ui {
class PandPTabWidget;
}

class PandPTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PandPTabWidget(QWidget *parent = 0);
    ~PandPTabWidget();
    int  addGroupTab(QString key, const QString &label, StageState currentState);

    void setCurrentWidget(QWidget *);
    bool addVariable(QString varName, QJsonObject JSONvariable, const QString &key, const QString &label , QString *setVal = NULL);

    int  index() { return activeIndex;};
    void setIndex(int );
    void setWidget(QWidget *);

    QWidget * currentWidget();
    QWidget * widget(int);

    int addVarTab(QString key, const QString &label);
    int addVarTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString, QString> *setVars);
    void addVarsToTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo, QMap<QString,QString> *setVars);
    void addVSpacer(const QString &key, const QString &label);

    void addVarsData(QJsonObject , QJsonObject );

    QMap<QString, QString> collectParamData();

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();
    void on_groupTabSelected(int);

signals:
    void switchToResultsTab();
    void switchToFileTab();
    void switchToCreateTab();
    void switchToParameterTab();
    void switchToHelpTab();

protected:
    QWidget * addStd(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addBool(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addFile(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addChoice(QJsonObject, QWidget *parent, QString *setVal = NULL);
    QWidget * addVector3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addVector2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addTensor3D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addTensor2D(QJsonObject, QWidget *parent, QString *setVal = NULL );
    QWidget * addUnknown(QJsonObject, QWidget *parent, QString *setVal = NULL );
    void addType(const QString &, const QString &, QJsonObject, QWidget *parent , QString * setVal);

    void setButtonMode(uint mode);

private:
    static QString getStateText(StageState theState);

    Ui::PandPTabWidget *ui;
    int activeIndex;
    QWidget *displayWidget;
    QMap<QString, CWE_WithStatusButton *> *groupWidget;
    QMap<QString, QTabWidget *> *groupTabList;
    QMap<QString, QMap<QString, QWidget *> *> *varTabWidgets;
    QMap<QString, InputDataType *> *variableWidgets;
};

#endif // PANDPTABWIDGET_H
