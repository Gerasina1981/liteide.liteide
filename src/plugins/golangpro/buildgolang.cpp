#include "buildgolang.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>


BuildGolang::BuildGolang(IApplication *app, QObject *parent) :
    liteApp(app), QObject(parent)
{
    runTarget = new RunTargetApp(app);

    buildOutput = new BuildOutputEdit;
    app->mainWindow()->addOutputPane(buildOutput,QIcon(),tr("Build Output"));

    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&process,SIGNAL(started()),this,SLOT(started()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));

    connect(buildOutput,SIGNAL(dbclickEvent()),this,SLOT(dbclickOutputEdit()));
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
    this->buildOutput->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutput);

    QStringList val = proj->values("TARGET");
    if (!val.isEmpty())
        target = val.at(0);
    else
        target = proj->displayName();

    target = QFileInfo(target).baseName();

    process.setWorkingDirectory(QFileInfo(proj->filePath()).absolutePath());

    QStringList args;
    args  << "-ver=false" << "-gopro" << proj->filePath();
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    process.start(cmd,args);

    return true;
}

bool BuildGolang::buildFile(const QString &fileName)
{
    target = QFileInfo(fileName).baseName();
    QString projDir = QFileInfo(fileName).absolutePath();
    process.setWorkingDirectory(projDir);

    QStringList args;
    args << "-ver=false" << "-gofiles" << QFileInfo(fileName).fileName() << "-o" << target;
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    process.start(cmd,args);

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

void BuildGolang::appendBuildOutput(const QString &text, bool stdError)
{
   // setCurrentOutputPane(buildOutputEdit);
    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);

    buildOutput->setCurrentCharFormat(fmt);
    buildOutput->appendPlainText(text);
}

void BuildGolang::finished(int code)
{
   // liteApp->buildEvent()->fireBuildStoped(code == 0);
    if (code == 0)
        appendBuildOutput("---- build finish ----",false);
    else
        appendBuildOutput("---- build error ----",false);
}

void BuildGolang::error(QProcess::ProcessError code)
{
   // liteApp->buildEvent()->fireBuildStoped(false);
    appendBuildOutput("build error",true);
}

void BuildGolang::readStdout()
{
    QString text = process.readAllStandardOutput();
    //liteApp->buildEvent()->fireBuildOutput(text,false);
    appendBuildOutput(text,false);
}
void BuildGolang::readStderr()
{
    QString text = process.readAllStandardError();
    //liteApp->buildEvent()->fireBuildOutput(text,true);
    appendBuildOutput(text,true);
}

void BuildGolang::started()
{
 //  liteApp->buildEvent()->fireBuildStarted();
    appendBuildOutput("---- build start ----",false);
}

void BuildGolang::dbclickOutputEdit()
{
    QTextCursor cur = buildOutput->textCursor();
    QRegExp rep("(\\w+[\\\\/])*(\\w+[.]\\w+)+:(\\d+)");
    int index = rep.indexIn(cur.block().text());
    if (index < 0)
        return;
    QStringList list = rep.capturedTexts();
    if (list.count() < 3)
        return;

    QString cap = list.at(0);
    QStringList capList = cap.split(":");
    if (capList.count() < 2) {
        return;
    }
    QString fileName = capList[0];
    QString fileLine = capList[1];
    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;

    cur.select(QTextCursor::LineUnderCursor);
    buildOutput->setTextCursor(cur);

    liteApp->gotoLine(fileName,line,0);
}
