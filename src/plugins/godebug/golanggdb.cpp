#include "golanggdb.h"
#include <QMenuBar>

GolangGdb::GolangGdb(IApplication *app, QObject *parent) :
    liteApp(app), QObject(parent)
{
    createActions();
    createMenus();
}

void GolangGdb::createActions()
{
    debugShellAct = new QAction(QIcon(":/images/build.png"),tr("Debug Shell"),this);
    debugShellAct->setShortcut(QKeySequence(Qt::Key_F5));
    debugShellAct->setStatusTip(tr("Start Debug Project"));
    connect(debugShellAct,SIGNAL(triggered()),this,SLOT(debugShell()));

    startDebugAct = new QAction(QIcon(":/images/build.png"),tr("Start Debug"),this);
    startDebugAct->setShortcut(QKeySequence(Qt::Key_F5));
    startDebugAct->setStatusTip(tr("Start Debug Project"));
    connect(startDebugAct,SIGNAL(triggered()),this,SLOT(startDebug()));

    stopDebugAct = new QAction(QIcon(":/images/build.png"),tr("Stop Debug"),this);
    stopDebugAct->setShortcut(QKeySequence(Qt::Key_Shift+Qt::Key_F5));
    stopDebugAct->setStatusTip(tr("Stop Debug Project"));
    connect(stopDebugAct,SIGNAL(triggered()),this,SLOT(stopDebug()));

}

void GolangGdb::createMenus()
{
    _debugMenu = liteApp->mainWindow()->widget()->menuBar()->addMenu("Debug");

    _debugMenu->addAction(startDebugAct);
    _debugMenu->addAction(stopDebugAct);
    _debugMenu->addSeparator();
    _debugMenu->addAction(debugShellAct);

    liteApp->mainWindow()->widget()->menuBar()->insertMenu(liteApp->mainWindow()->toolMenu()->menuAction(),_debugMenu);

}

void GolangGdb::startDebug()
{
    //target = liteApp->getTargetInfo();
    TargetInfo info = liteApp->getTargetInfo();
    debugProcess.setWorkingDirectory(info.workDir);
    QStringList args;
    args << "/K" << info.fileName << "| pause" << "exit";
    //debugProcess.start("cmd c:/Mingw-4.4.1/bin/gdb",args,QIODevice::NotOpen);
    //QProcess::startDetached("c:/Mingw-4.4.1/bin/gdb",args);
    QProcess::startDetached("cmd",args);
}

void GolangGdb::stopDebug()
{
}

void GolangGdb::debugShell()
{
    TargetInfo info = liteApp->getTargetInfo();
    debugProcess.setWorkingDirectory(info.workDir);
    QStringList args;
    args << info.fileName;
    QProcess::startDetached("c:/Mingw-4.4.1/bin/gdb",args,info.workDir);
}
