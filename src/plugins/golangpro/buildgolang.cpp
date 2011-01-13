#include "buildgolang.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

BuildGolang::BuildGolang(IApplication *app, QObject *parent) :
    liteApp(app), QObject(parent)
{
    runTarget = new RunTargetApp(app);
    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&process,SIGNAL(started()),this,SLOT(started()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
}

BuildGolang::~BuildGolang()
{
    if (runTarget) {
        delete runTarget;
    }
}

void BuildGolang::setActive()
{
    liteApp->setRunTarget(runTarget);
}

QString BuildGolang::buildName() const
{
    return tr("BuildGoalng");
}

bool BuildGolang::buildProject(IProject *proj)
{
    QStringList val = proj->values("TARGET");
    if (!val.isEmpty())
        target = val.at(0);
    else
        target = proj->displayName();

    target = QFileInfo(target).baseName();

    process.setWorkingDirectory(QFileInfo(proj->filePath()).absolutePath());

    QStringList args;
    args << "-gopro" << proj->filePath() << "-ver=false";
    process.start("gopromake.exe",args);

    return true;
}

bool BuildGolang::buildFile(const QString &fileName)
{
    target = QFileInfo(fileName).baseName();
    QString projDir = QFileInfo(fileName).absolutePath();
    process.setWorkingDirectory(projDir);

    QStringList args;
    args << "-ver=false" << "-gofiles" << QFileInfo(fileName).fileName() << "-out" << target;
    process.start("gopromake.exe",args);

    return false;
}

void BuildGolang::cancelBuild()
{
    if (process.state() == QProcess::Starting) {
        process.waitForStarted(3000);
    } else if (process.state() == process.Running) {
        process.waitForFinished(3000);
    }
}

void BuildGolang::finished(int code)
{
    liteApp->buildEvent()->fireBuildStoped(code == 0);
}

void BuildGolang::error(QProcess::ProcessError code)
{
    liteApp->buildEvent()->fireBuildStoped(false);
}

void BuildGolang::readStdout()
{
    QString text = process.readAllStandardOutput();
    liteApp->buildEvent()->fireBuildOutput(text,false);
}
void BuildGolang::readStderr()
{
    QString text = process.readAllStandardError();
    liteApp->buildEvent()->fireBuildOutput(text,true);
}

void BuildGolang::started()
{
   liteApp->buildEvent()->fireBuildStarted();
}
