#-------------------------------------------------
#
# Project created by QtCreator 2011-01-20T16:25:00
#
#-------------------------------------------------
include (../../liteide.pri)
include (../../api/api.pri)
include (../../util/util.pri)

TARGET = goastview
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH


DEFINES += GOASTVIEW_LIBRARY

SOURCES += goastviewplugin.cpp \
    goastviewmanager.cpp \
    goastview.cpp

HEADERS += goastviewplugin.h\
        goastview_global.h \
    goastviewmanager.h \
    goastview.h

RESOURCES += \
    goastview.qrc
