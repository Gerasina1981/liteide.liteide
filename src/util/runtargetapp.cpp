#include "runtargetapp.h"
#include "../api/iproject.h"
#include "../api/ieditor.h"

#include <QFileInfo>
#include <QDir>
#include <QDebug>

RunTargetApp::RunTargetApp(IApplication *app, QObject *parent)
    : liteApp(app), QObject(parent)
{
    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&process,SIGNAL(started()),this,SLOT(started()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
}

void RunTargetApp::runProject(IProject *proj)
{
    QStringList val = proj->values("TARGET");
    QString target = proj->displayName();
    if (!val.isEmpty())
        target = val.at(0);

    target = QFileInfo(target).baseName();
    QString projDir = QFileInfo(proj->filePath()).absolutePath();
    process.setWorkingDirectory(projDir);
    target = QFileInfo(QDir(projDir),target+".exe").absoluteFilePath();
    process.start(target);
}

void RunTargetApp::runEditor(IEditor *edit)
{

}


void RunTargetApp::finished(int code)
{
    liteApp->runTargetEvent()->fireRunTargetStoped(code == 0);
}

void RunTargetApp::error(QProcess::ProcessError code)
{
    liteApp->runTargetEvent()->fireRunTargetStoped(false);
}

void RunTargetApp::readStdout()
{
    QByteArray text = process.readAllStandardOutput();
    liteApp->runTargetEvent()->fireRunTargetOutput(text,false);
}
void RunTargetApp::readStderr()
{
    QByteArray text = process.readAllStandardError();
    liteApp->runTargetEvent()->fireRunTargetOutput(text,true);
}

void RunTargetApp::started()
{
    liteApp->runTargetEvent()->fireRunTargetStarted();
}
