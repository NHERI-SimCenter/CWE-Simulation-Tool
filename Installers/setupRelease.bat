REM @ECHO OFF
echo "Preparing redistributable package for 64-bit MSVC release"

RD /S /Q release
RD /S /Q installer

MKDIR release

COPY ..\qtProjects\build-CFDClientProgram-Desktop_Qt_5_9_0_MSVC2017_64bit-Release\release\*.exe release\

set PATH=%PATH%;D:\QT\5.9\msvc2017_64\bin\

windeployqt release\CFDClientProgram.exe --no-translations

XCOPY /S /I dataStore installer

XCOPY /S /I release\* installer\packages\nheri.simcenter.cfdclient\data

cd installer

D:\QT\Tools\QtInstallerFramework\2.0\bin\binarycreator.exe -c config\config.xml -p packages setupSimCenterCFD.exe

PAUSE
