#!/bin/bash

#----- adjust user settings -----------------------------

QT_HOME=/home/psempoli/Qt/5.9/gcc_64/lib
QT_PLUGINS=/home/psempoli/Qt/5.9/gcc_64/plugins

#----- do not modify below here -------------------------

rm -rf release
rm -rf installer

mkdir release
mkdir release/exec

cp ../qtProjects/build-CFDClientProgram-Desktop_Qt_5_9_0_GCC_64bit-Release/CFDClientProgram release/exec

mkdir release/plugins
mkdir release/plugins/platforms
mkdir release/plugins/xcbglintegrations

cp $QT_PLUGINS/platforms/libqxcb.so release/plugins/platforms
cp $QT_PLUGINS/xcbglintegrations/libqxcb-egl-integration.so release/plugins/xcbglintegrations/libqxcb-egl-integration.so
cp $QT_PLUGINS/xcbglintegrations/libqxcb-glx-integration.so release/plugins/xcbglintegrations/libqxcb-glx-integration.so

cp dataStore/RunSimCenterCFD release/RunSimCenterCFD.sh


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

cp -R dataStore/packages installer
cp -R dataStore/config installer
cp -R release/* installer/packages/nheri.simcenter.cfdclient/data/

cd installer

/home/psempoli/Qt/Tools/QtInstallerFramework/2.0/bin/binarycreator --offline-only -c config/config.xml -p packages InstallSimCenterCFD
