#!/bin/sh

mkdir liteide.app/Contents/MacOS/plugins

cp -R template liteide.app/Contents/MacOS
cp Info.plist liteide.app/Contents
cp goastview liteide.app/Contents/MacOS
cp gopromake liteide.app/Contents/MacOS

function process_file()
{
cp -R plugins/$deploy_file liteide.app/Contents/MacOS/plugins/

install_name_tool -change \
 QtCore.framework/Versions/4/QtCore \
 @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
 liteide.app/Contents/MacOS/plugins/$deploy_file  

install_name_tool -change \
 QtGui.framework/Versions/4/QtGui \
 @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
  liteide.app/Contents/MacOS/plugins/$deploy_file   
}

export deploy_file=libgolangpro.1.0.0.dylib
process_file

export deploy_file=libproject.1.0.0.dylib
process_file

export deploy_file=libgoastview.1.0.0.dylib
process_file

export deploy_file=libsyntaxeditor.1.0.0.dylib
process_file

macdeployqt liteide.app -dmg
