#ifndef TESTFRAME_H
#define TESTFRAME_H

#include <QFrame>

namespace Ui {
class TestFrame;
}

class TestFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TestFrame(QWidget *parent = 0);
    ~TestFrame();

private:
    Ui::TestFrame *ui;
};

#endif // TESTFRAME_H
