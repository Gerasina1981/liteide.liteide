#-------------------------------------------------
#
# Project created by QtCreator 2010-12-30T11:01:57
#
#-------------------------------------------------
include (../../liteide.pri)
include (../../api/liteapi.pri)

TARGET = buildgo
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += BUILDGO_LIBRARY

SOURCES += buildgoplguin.cpp

HEADERS += buildgoplguin.h\
        buildgo_global.h

INSTALLS += target
