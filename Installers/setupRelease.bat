REM @ECHO OFF
echo "Preparing redistributable package for 64-bit MSVC release"

REM To Use:    1) Go to an empty folder for creating the install package
REM 		2) Invoke this script, passing the folder of the compiled release
REM 		Example: ..\qtProjects\CFDClientProgram\Installers\setupRelease.bat ..\qtProjects\build-CFDClientProgram-Desktop_Qt_5_9_2_MSVC2017_64bit-Release\release

SET MADE_EXEC_DIR=%1
SET GENERATOR_DIR=%~dp0

rem ----- adjust user settings -----------------------------

SET QT_WIN_DEPLOY=D:\QT\5.10.1\msvc2017_64\bin\windeployqt
SET QT_BIN_CREATE=D:\QT\Tools\QtInstallerFramework\3.0\bin\binarycreator.exe

rem ----- do not modify below here -------------------------

RD /S /Q release
RD /S /Q installer

MKDIR release

COPY %MADE_EXEC_DIR%\CFDClientProgram.exe release\
XCOPY /S /I %MADE_EXEC_DIR%\resources release\resources

%QT_WIN_DEPLOY% release\CFDClientProgram.exe --no-translations

MKDIR installer
MKDIR installer\packages 
MKDIR installer\config

XCOPY /S /I %GENERATOR_DIR%\dataStore\packages installer\packages
XCOPY /S /I %GENERATOR_DIR%\dataStore\config installer\config

XCOPY /S /I release\* installer\packages\nheri.simcenter.cfdclient\data

cd installer

%QT_BIN_CREATE% --offline-only -c config\winconfig.xml -p packages ../setupSimCenterCFD.exe

PAUSE

cd ..
