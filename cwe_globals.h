#ifndef CWE_GLOBALS_H
#define CWE_GLOBALS_H

#include <QString>
#include <QMessageBox>

class cwe_globals
{
public:
    cwe_globals();
    static void displayPopup(QString message, QString header);
    static void displayPopup(QString message);
};

#endif // CWE_GLOBALS_H
