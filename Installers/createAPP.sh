#!/bin/bash

#----- adjust user settings -----------------------------

APPNAME=CFDClientProgram.app
APPDIR=../../../build-CFDClientProgram-Desktop_Qt_5_9_0_clang_64bit-Release/
APPINSTALLER=SetupSimCenterCFDClient

MACDEPLOY=${HOME}/Qt/5.9/clang_64/bin/macdeployqt

QTINSTALLER=${HOME}/Qt/Tools/QtInstallerFramework/2.0/bin/binarycreator
QTINSTALLEROPTIONS="--offline-only --ignore-translations -c config/macconfig.xml -p packages"


#----- do not modify below here -------------------------

rm -rf release
rm -rf installer

cp -R dataStore installer
mkdir release

cp -R ${APPDIR}${APPNAME} release

${MACDEPLOY} release/${APPNAME}

cp -R release/${APPNAME} installer/packages/nheri.simcenter.cfdclient/data

cd installer

echo ${QTINSTALLER} ${QTINSTALLEROPTIONS} ${APPINSTALLER}
${QTINSTALLER} ${QTINSTALLEROPTIONS} ${APPINSTALLER}

