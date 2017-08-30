#ifndef PANDPTABWIDGET_H
#define PANDPTABWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMap>
#include <QList>

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
    bool addVariable(QJsonObject, const QString &key, const QString &label );

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

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();
    void on_groupTabSelected(int);

protected:
    void addStd(QJsonObject, QWidget *parent );
    void addBool(QJsonObject, QWidget *parent );
    void addFile(QJsonObject, QWidget *parent );
    void addChoice(QJsonObject, QWidget *parent );
    void addUnknown(QJsonObject, QWidget *parent );
    void addType(const QString, QJsonObject, QWidget *parent );

private:
    Ui::PandPTabWidget *ui;
    int activeIndex;
    QWidget *displayWidget;
    QMap<QString, QWidget *> *groupWidget;
    QMap<QString, QWidget *> *groupTabList;
    QMap<QString, QWidget *> *variableWidgets;
    QMap<QString, QMap<QString, QWidget *> *> *varTabWidgets;
};

#endif // PANDPTABWIDGET_H
