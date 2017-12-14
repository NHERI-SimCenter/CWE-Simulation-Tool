#ifndef CWE_STAGESTATUSTAB_H
#define CWE_STAGESTATUSTAB_H

#include <QFrame>
//#include <QAbstractButton>

class CWE_GroupsWidget;

namespace Ui {
class CWE_StageStatusTab;
}

class CWE_StageStatusTab : public QFrame
{
    Q_OBJECT

public:
    explicit CWE_StageStatusTab(QString stageName, QWidget *parent = 0);
    ~CWE_StageStatusTab();
    void setCorrespondingPanel(CWE_GroupsWidget * newPanel);

    void setStatus(QString);
    void setText(QString);
    void setName(const QString s);
    //void setIndex(int idx) {m_index = idx;};
    QString name() {return m_name;};
    QString text() {return m_text;};
    QString status() {return m_status;};
    //int index() {return m_index;};
    void setActive(bool b=true);
    void setInActive(bool b=true);
    void linkWidget(CWE_GroupsWidget *ptr);

signals:
    void btn_pressed(CWE_GroupsWidget *, QString);
    void btn_released(CWE_GroupsWidget *);
    void btn_activated(CWE_StageStatusTab *);

private slots:

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::CWE_StageStatusTab *ui;
    //void paintEvent(QPaintEvent*);

    CWE_GroupsWidget * myPanel = NULL;

    QString m_text;
    QString m_status = "unknown";
    QString m_name = "label text";
    //int m_index = -1;
    bool m_active;
};

#endif // CWE_STAGESTATUSTAB_H
