#!/bin/bash

#To Use:    1) Go to an empty folder for creating the install package
#           2) Invoke this script, passing the folder of the compiled release
#           Example: ../qtProjects/CFDClientProgram/Installers/generateInstall.sh ../qtProjects/build-CFDClientProgram-Desktop_Qt_5_9_2_GCC_64bit-Release/

MADE_EXEC_DIR=$1
GENERATOR_DIR=$(dirname "$0")

#----- adjust user settings -----------------------------

QT_HOME=~/Qt/5.10.1/gcc_64/lib
QT_PLUGINS=~/Qt/5.10.1/gcc_64/plugins
QT_BIN_CREATE=~/Qt/Tools/QtInstallerFramework/3.0/bin/binarycreator

#----- do not modify below here -------------------------

rm -rf release
rm -rf installer

mkdir release
mkdir release/exec

cp $MADE_EXEC_DIR/CWE-Simulation-Tool release/exec
cp -R $MADE_EXEC_DIR/resources release/exec

mkdir release/plugins
mkdir release/plugins/platforms
mkdir release/plugins/xcbglintegrations

cp $QT_PLUGINS/platforms/libqxcb.so release/plugins/platforms
cp $QT_PLUGINS/xcbglintegrations/libqxcb-egl-integration.so release/plugins/xcbglintegrations/libqxcb-egl-integration.so
cp $QT_PLUGINS/xcbglintegrations/libqxcb-glx-integration.so release/plugins/xcbglintegrations/libqxcb-glx-integration.so

cp $GENERATOR_DIR/dataStore/RunSimCenterCWE release/RunSimCenterCWE.sh
chmod 744 release/RunSimCenterCWE.sh

mkdir release/lib

cp $QT_HOME/libQt5Widgets.so.5 release/lib
cp $QT_HOME/libQt5Gui.so.5 release/lib
cp $QT_HOME/libQt5Network.so.5 release/lib
cp $QT_HOME/libQt5Core.so.5 release/lib
cp $QT_HOME/libicui18n.so.56 release/lib
cp $QT_HOME/libicuuc.so.56 release/lib

cp $QT_HOME/libicudata.so.56 release/lib
cp $QT_HOME/libQt5XcbQpa.so.5 release/lib
cp $QT_HOME/libQt5DBus.so.5 release/lib

mkdir installer

cp -R $GENERATOR_DIR/dataStore/packages installer
cp -R $GENERATOR_DIR/dataStore/config installer
mkdir installer/packages/nheri.simcenter.cfdclient/data
cp -R release/* installer/packages/nheri.simcenter.cfdclient/data/

cd installer

$QT_BIN_CREATE --offline-only -c config/linconfig.xml -p packages ../InstallSimCenterCFD
