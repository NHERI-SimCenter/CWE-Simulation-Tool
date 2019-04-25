#ifndef INFLOWPARAMETERWIDGET_H
#define INFLOWPARAMETERWIDGET_H

#include <QFrame>

namespace Ui {
class InflowParameterWidget;
}

class InflowParameterWidget : public QFrame
{
    Q_OBJECT

public:
    explicit InflowParameterWidget(QWidget *parent = nullptr);
    ~InflowParameterWidget();

private slots:
    void on_btnNormalize_clicked();

    void on_PHI21_valueChanged(double arg1);
    void on_PHI31_valueChanged(double arg1);
    void on_PHI32_valueChanged(double arg1);

private:
    Ui::InflowParameterWidget *ui;
};

#endif // INFLOWPARAMETERWIDGET_H
