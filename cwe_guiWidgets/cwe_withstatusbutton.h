#ifndef CWE_WITHSTATUSBUTTON_H
#define CWE_WITHSTATUSBUTTON_H

#include <QFrame>

namespace Ui {
class CWE_WithStatusButton;
}

class CWE_WithStatusButton : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_WithStatusButton(QWidget *parent = 0);
    ~CWE_WithStatusButton();
    void setStatus(QString);
    void setText(QString);
    void setName(const QString s) {m_name = s;};
    QString name() {return m_name;};
    QString text() {return m_text;};
    QString status() {return m_status;};

signals:
    void btn_clicked();

private slots:
    void on_statusLabel_linkActivated(const QString &link);
    void on_mainLabel_linkActivated(const QString &link);

private:
    Ui::CWE_WithStatusButton *ui;
    void setSelected(bool);

    QString m_text;
    QString m_status = "unknown";
    QString m_name = "label text";
    bool selected;
};

#endif // CWE_WITHSTATUSBUTTON_H
