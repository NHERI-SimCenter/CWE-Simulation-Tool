#ifndef PANDPTABWIDGET_H
#define PANDPTABWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>
#include <QTabWidget>
#include "cwe_defines.h"
#include "cwe_withstatusbutton.h"

namespace Ui {
class PandPTabWidget;
}

class PandPTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PandPTabWidget(QWidget *parent = 0);
    ~PandPTabWidget();
    int  addGroupTab(QString key, const QString &label);

    void setCurrentWidget(QWidget *);
    bool addVariable(QString varName, QJsonObject JSONvariable, const QString &key, const QString &label );

    int  index() { return activeIndex;};
    void setIndex(int );
    void setWidget(QWidget *);

    QWidget * currentWidget();
    QWidget * widget(int);

    int  addVarTab(QString key, const QString &label);
    int  addVarTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo);
    void addVarsToTab(QString key, const QString &label, QJsonArray *varList, QJsonObject *varsInfo);
    void addVSpacer(const QString &key, const QString &label);

    void addVarsData(QJsonObject , QJsonObject );

signals:
    void run_analysis_on_design_safe_pressed(QMap<QString, QString> *);
    void cancel_analysis_on_design_safe_pressed();
    void reset_analysis_on_design_safe_pressed(QMap<QString, QString> *);
    //void _analysis_on_design_safe_pressed(QMap<QString, QString> *);

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();
    void on_groupTabSelected(int);

protected:
    QWidget * addStd(QJsonObject, QWidget *parent );
    QWidget * addBool(QJsonObject, QWidget *parent );
    QWidget * addFile(QJsonObject, QWidget *parent );
    QWidget * addChoice(QJsonObject, QWidget *parent );
    QWidget * addVector3D(QJsonObject, QWidget *parent );
    QWidget * addVector2D(QJsonObject, QWidget *parent );
    QWidget * addTensor3D(QJsonObject, QWidget *parent );
    QWidget * addTensor2D(QJsonObject, QWidget *parent );
    QWidget * addUnknown(QJsonObject, QWidget *parent );
    void addType(const QString &, const QString &, QJsonObject, QWidget *parent );

private:
    Ui::PandPTabWidget *ui;
    int activeIndex;
    QWidget *displayWidget;
    QMap<QString, CWE_WithStatusButton *> *groupWidget;
    QMap<QString, QTabWidget *> *groupTabList;
    QMap<QString, QMap<QString, QWidget *> *> *varTabWidgets;
    QMap<QString, InputDataType *> *variableWidgets;
};

#endif // PANDPTABWIDGET_H
