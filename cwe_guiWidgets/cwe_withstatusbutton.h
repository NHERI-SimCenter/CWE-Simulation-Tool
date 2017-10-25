#ifndef CWE_WITHSTATUSBUTTON_H
#define CWE_WITHSTATUSBUTTON_H

#include <QFrame>
//#include <QAbstractButton>

namespace Ui {
class CWE_WithStatusButton;
}

class CWE_WithStatusButton : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_WithStatusButton(QString stageName, QWidget *parent = 0);
    ~CWE_WithStatusButton();
    void setStatus(QString);
    void setText(QString);
    void setName(const QString s) {m_name = s;};
    void setIndex(int idx) {m_index = idx;};
    QString name() {return m_name;};
    QString text() {return m_text;};
    QString status() {return m_status;};
    int index() {return m_index;};
    void setActive(bool b=true);
    void setInActive(bool b=true);

signals:
    void btn_pressed(int, QString);
    void btn_released(int);

private slots:

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::CWE_WithStatusButton *ui;
    //void paintEvent(QPaintEvent*);

    QString internal_name;

    QString m_text;
    QString m_status = "unknown";
    QString m_name = "label text";
    int m_index = -1;
    bool m_active;
};

#endif // CWE_WITHSTATUSBUTTON_H
