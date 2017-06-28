#!/bin/bash

#----- adjust user settings -----------------------------

APPNAME=CFDClientProgram.app
APPDIR=../../../../build-CFDClientProgram-Desktop_Qt_5_7_0_clang_64bit2-Release/
#APPDIR=../../../../build-CFDClientProgram-Desktop_Qt_5_7_0_clang_64bit2-Debug/
APPINSTALLER=SetupSimCenterCFDClient

MACDEPLOY=${HOME}/Qt/5.7/clang_64/bin/macdeployqt

QTINSTALLER=${HOME}/Qt/Tools/QtInstallerFramework/2.0/bin/binarycreator
QTINSTALLEROPTIONS="--offline-only --ignore-translations -c config/config.xml -p packages"


#----- do not modify below here -------------------------

rm -rf release
rm -rf installer

cp -R dataStore installer
mkdir release

cp -R ${APPDIR}${APPNAME} release

${MACDEPLOY} release/${APPNAME}

cp -R release/${APPNAME} installer/packages/com.vendor.product/data

cd installer

${QTINSTALLER} ${QTINSTALLEROPTIONS} ${APPINSTALLER}

