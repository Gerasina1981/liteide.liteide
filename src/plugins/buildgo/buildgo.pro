#-------------------------------------------------
#
# Project created by QtCreator 2010-12-30T11:01:57
#
#-------------------------------------------------
include (../../liteide.pri)
include (../../api/api.pri)
include (../../util/util.pri)

TARGET = buildgo
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += BUILDGO_LIBRARY

SOURCES += buildgoplguin.cpp \
    buildgolang.cpp

HEADERS += buildgoplguin.h\
        buildgo_global.h \
    buildgolang.h

INSTALLS += target
