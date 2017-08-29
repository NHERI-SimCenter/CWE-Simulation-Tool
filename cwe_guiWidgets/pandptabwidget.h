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
    int addGroupTab(QString key, const QString &label);
    void addVarsData(QJsonObject , QJsonObject );
    void setCurrentWidget(QWidget *);
    QWidget * currentWidget();
    QWidget * widget(int);
    void addStd(QJsonObject );
    void addBool(QJsonObject );
    void addFile(QJsonObject );
    void addChoice(QJsonObject );
    void addUnknown(QJsonObject );
    void addType(const QString, QJsonObject );

    int index() { return activeIndex;};
    void setIndex(int );
    void setWidget(QWidget *);

    int addVarTab(QString key, const QString &label);

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();

private:
    Ui::PandPTabWidget *ui;
    int activeIndex;
    QWidget *displayWidget;
    QMap<QString, QWidget *> *groupWidget;
    QMap<QString, QList<QWidget *> *>  *groupTabList;
};

#endif // PANDPTABWIDGET_H
