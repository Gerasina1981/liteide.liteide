#-------------------------------------------------
#
# Project created by QtCreator 2011-02-18T09:25:50
#
#-------------------------------------------------
include (../../liteide.pri)

TARGET = godoc
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += GODOC_LIBRARY

SOURCES += godocplugin.cpp \
    godocview.cpp

HEADERS += godocplugin.h\
        godoc_global.h \
    godocview.h
