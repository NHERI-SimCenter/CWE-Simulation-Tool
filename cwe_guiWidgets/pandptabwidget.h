#ifndef PANDPTABWIDGET_H
#define PANDPTABWIDGET_H

#include <QWidget>

namespace Ui {
class PandPTabWidget;
}

class PandPTabWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PandPTabWidget(QWidget *parent = 0);
    ~PandPTabWidget();
    int addTab(QWidget *page, const QString &label);
    void setCurrentWidget(QWidget *);
    QWidget * currentWidget();
    QWidget * widget(int);

private slots:
    void on_pbtn_run_clicked();
    void on_pbtn_cancel_clicked();
    void on_pbtn_results_clicked();
    void on_pbtn_rollback_clicked();

private:
    Ui::PandPTabWidget *ui;
};

#endif // PANDPTABWIDGET_H
