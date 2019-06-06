#ifndef DIALOGINFLOWPARAMETERS_H
#define DIALOGINFLOWPARAMETERS_H

#include <QDialog>

namespace Ui {
class DialogInflowParameters;
}

class DialogInflowParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInflowParameters(QWidget *parent = nullptr);
    ~DialogInflowParameters();

private:
    Ui::DialogInflowParameters *ui;
};

#endif // DIALOGINFLOWPARAMETERS_H
