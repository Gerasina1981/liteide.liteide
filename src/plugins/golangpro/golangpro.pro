#-------------------------------------------------
#
# Project created by QtCreator 2010-12-28T23:14:03
#
#-------------------------------------------------
include (../../liteide.pri)
include (../../api/api.pri)
include (../../util/util.pri)

TARGET = golangpro
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += GOLANGPRO_LIBRARY

SOURCES += golangproplugin.cpp \
    buildgolang.cpp \
    golangoption.cpp

HEADERS += golangproplugin.h\
        golangpro_global.h \
    buildgolang.h \
    golangoption.h

FORMS += \
    golangoption.ui

RESOURCES += \
    golangpro.qrc
