#-------------------------------------------------
#
# Project created by QtCreator 2010-12-28T23:14:03
#
#-------------------------------------------------
include (../../liteide.pri)

TARGET = golangpro
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += GOLANGPRO_LIBRARY

SOURCES += golangproplugin.cpp

HEADERS += golangproplugin.h\
        golangpro_global.h

INSTALLS += target
