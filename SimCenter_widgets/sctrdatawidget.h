#ifndef SCTRDATAWIDGET_H
#define SCTRDATAWIDGET_H

#include <QWidget>

enum class SimCenterViewState { visible, editable, hidden };

namespace Ui {
class SCtrDataWidget;
}

class SCtrDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SCtrDataWidget(QWidget *parent = 0);
    ~SCtrDataWidget();
    SimCenterViewState ViewState();
    void setViewState(SimCenterViewState);
    void setData(QJsonObject &);
    void setValue(QString);
    void setValue(float);
    void setValue(int);
    void setValue(bool);
    QString Value();

private slots:
    void on_txted_theValue_textChanged();

private:
    Ui::SCtrDataWidget *ui;
    SimCenterViewState m_ViewState;
};

#endif // SCTRDATAWIDGET_H
