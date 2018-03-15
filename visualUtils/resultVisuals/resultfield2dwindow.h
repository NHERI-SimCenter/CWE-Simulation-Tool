#ifndef RESULTFIELD2DWINDOW_H
#define RESULTFIELD2DWINDOW_H

#include "visualUtils/resultvisualpopup.h"

class ResultField2dWindow : public ResultVisualPopup
{
public:
    ResultField2dWindow(CFDcaseInstance * theCase, QMap<QString, QString> resultDesc, QWidget *parent = 0);
    ~ResultField2dWindow();

    virtual void initializeView();

private:
    virtual void allFilesLoaded();
};

#endif // RESULTFIELD2DWINDOW_H
