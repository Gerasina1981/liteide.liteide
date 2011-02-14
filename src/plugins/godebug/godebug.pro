#-------------------------------------------------
#
# Project created by QtCreator 2011-02-14T15:40:33
#
#-------------------------------------------------
include (../../liteide.pri)

TARGET = godebug
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += GODEBUG_LIBRARY

SOURCES += godebugplugin.cpp

HEADERS += godebugplugin.h\
        godebug_global.h
