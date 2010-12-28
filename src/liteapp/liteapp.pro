# -------------------------------------------------
# Project created by QtCreator 2010-12-27T19:44:39
# -------------------------------------------------
include(../liteide.pri)

TARGET = $$IDE_APP_TARGET
#CONFIG += app
#CONFIG -= app_bundle
DEFINES += LITEAPP_LIBRARY
DESTDIR = $$IDE_APP_PATH
TEMPLATE = app
SOURCES += main.cpp \
    liteapp.cpp \
    mainwindow.cpp
HEADERS += liteapp.h \
    mainwindow.h

RESOURCES += liteapp.qrc

INSTALLS += target
