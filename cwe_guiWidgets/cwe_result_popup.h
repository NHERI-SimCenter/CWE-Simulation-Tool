#ifndef CWE_RESULT_POPUP_H
#define CWE_RESULT_POPUP_H

#include <QWidget>
#include <QLabel>

class VWTinterfaceDriver;
class CFDglCanvas;

namespace Ui {
class CWE_Result_Popup;
}

class CWE_Result_Popup : public QWidget
{
    Q_OBJECT

public:
    explicit CWE_Result_Popup(QString caseName, QString caseType, QMap<QString, QString> theResult, VWTinterfaceDriver * theDriver , QWidget *parent = 0);
    ~CWE_Result_Popup();

private slots:
    void closeButtonClicked();
    void newFileInfo();

private:
    Ui::CWE_Result_Popup *ui;
    VWTinterfaceDriver * myDriver;

    QString resultType;
    QString targetFolder;

    QLabel * loadingLabel = NULL;
    CFDglCanvas * myCanvas = NULL;
};

#endif // CWE_RESULT_POPUP_H
