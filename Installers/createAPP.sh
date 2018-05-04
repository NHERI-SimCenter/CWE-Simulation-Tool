#!/bin/bash

#To Use:    1) Go to an empty folder for creating the install package
#           2) Invoke this script, passing the folder of the compiled release
#           Example: ../qtProjects/CFDClientProgram/Installers/createAPP.sh ../qtProjects/build-CFDClientProgram-Desktop_Qt_5_9_2_clang_64bit-Release/


MADE_EXEC_DIR=$1
GENERATOR_DIR=$(dirname "$0")

#----- adjust user settings -----------------------------

APPNAME=CWE-Simulation-Tool.app
APPINSTALLER=SetupSimCenterCWE

MACDEPLOY=${HOME}/Qt/5.10.1/clang_64/bin/macdeployqt

QTINSTALLER=${HOME}/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator
QTINSTALLEROPTIONS="--offline-only --ignore-translations -c config/macconfig.xml -p packages"

#----- do not modify below here -------------------------

rm -rf release
rm -rf installer

cp -R $GENERATOR_DIR/dataStore installer
mkdir release

cp -R ${MADE_EXEC_DIR}/${APPNAME} release

${MACDEPLOY} release/${APPNAME}

mkdir installer/packages/nheri.simcenter.cfdclient/data

cp -R release/${APPNAME} installer/packages/nheri.simcenter.cfdclient/data/

cd installer

echo ${QTINSTALLER} ${QTINSTALLEROPTIONS} ${APPINSTALLER}
${QTINSTALLER} ${QTINSTALLEROPTIONS} ../${APPINSTALLER}

