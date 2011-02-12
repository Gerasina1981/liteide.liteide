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
    target = proj->displayName();
    if (!val.isEmpty())
        target = val.at(0);

    target = QFileInfo(target).baseName();
    QString projDir = QFileInfo(proj->filePath()).absolutePath();
    QStringList dest = proj->values("DESTDIR");
    if (!dest.isEmpty()) {
        projDir = QFileInfo(QFileInfo(proj->filePath()).absoluteDir(),dest.at(0)).absoluteFilePath();
    }
    process.setWorkingDirectory(projDir);
    target = QFileInfo(QDir(projDir),target+liteApp->osExecuteExt()).absoluteFilePath();
    process.start(target);
}

void RunTargetApp::runEditor(IEditor *edit)
{
    target = QFileInfo(edit->filePath()).baseName();
    QString projDir = QFileInfo(edit->filePath()).absolutePath();
    process.setWorkingDirectory(projDir);
    target = QFileInfo(QDir(projDir),target+liteApp->osExecuteExt()).absoluteFilePath();
    process.start(target);
}


void RunTargetApp::finished(int code)
{
    QString text = QString("%1 exited with code %2").arg(target).arg(code);
}

void RunTargetApp::error(QProcess::ProcessError code)
{
    QString text = QString("\nStart %1 faild!").arg(target);
}

void RunTargetApp::readStdout()
{
    QByteArray text = process.readAllStandardOutput();
}
void RunTargetApp::readStderr()
{
    QByteArray text = process.readAllStandardError();
}

void RunTargetApp::started()
{
    QString text = QString("Starting %1 ...").arg(target);
}
