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
    if (proj->values("GOFILES").isEmpty()) {
        liteApp->buildEvent()->fireBuildStoped(0);
        return false;
    }

    liteApp->buildEvent()->fireBuildOutput("\nbuild project "+proj->displayName(),false);

    QStringList val = proj->values("TARGET");
    if (!val.isEmpty())
        target = val.at(0);
    else
        target = proj->displayName();

    target = QFileInfo(target).baseName();

    build8g = true;
    build8l = false;

    QString projDir = QFileInfo(proj->filePath()).absolutePath();
    process.setWorkingDirectory(projDir);

    QStringList args;
    args << "-o" << target+"_go_.8" << proj->values("GOFILES");
    liteApp->buildEvent()->fireBuildOutput("8g "+args.join(" "),false);
    process.start("c:\\go\\bin\\8g.exe",args);
    return true;
}

bool BuildGolang::buildEditor(IEditor *edit)
{
    return false;
}

void BuildGolang::cancelBuild()
{
    build8g = false;
    if (process.state() == QProcess::Starting) {
        process.waitForStarted(3000);
    } else if (process.state() == process.Running) {
        process.waitForFinished(3000);
    }
}

void BuildGolang::finished(int code)
{
    if (build8g == true) {
        build8g = false;
        liteApp->buildEvent()->fireBuildOutput("--- 8g ended! ---",false);
        if (code == 0) {
            build8l = true;
            QStringList args;
            args << "-o" << target+".exe" << target+"_go_.8";
            liteApp->buildEvent()->fireBuildOutput("8l "+args.join(" "),false);
            process.start("c:\\go\\bin\\8l.exe",args);
            return;
        }
    } else if (build8l == true) {
        build8l = false;
        liteApp->buildEvent()->fireBuildOutput("--- 8l ended !",false);
    }
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
    if (build8g) {
        liteApp->buildEvent()->fireBuildStarted();
    }
}
