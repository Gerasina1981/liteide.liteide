#-------------------------------------------------
#
# Project created by QtCreator 2010-12-29T11:59:20
#
#-------------------------------------------------
include (../../liteide.pri)

TARGET = project
TEMPLATE = lib

DESTDIR = $$IDE_PLGUINS_PATH

DEFINES += PROJECT_LIBRARY

SOURCES += projectplugin.cpp \
    projectfile.cpp \
    projectmanager.cpp

HEADERS += projectplugin.h\
        project_global.h \
    projectfile.h \
    projectmanager.h
