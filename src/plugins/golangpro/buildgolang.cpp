#include "buildgolang.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QTextCodec>
#include "golangoption.h"

/*
enum ProcessError {
    FailedToStart, //### file not found, resource error
    Crashed,
    Timedout,
    ReadError,
    WriteError,
    UnknownError
};
*/
static QByteArray processErrorName(QProcess::ProcessError code)
{
    static QByteArray err;
    switch (code) {
    case QProcess::FailedToStart:
        err = "File not found, resouce error";
        break;
    case QProcess::Crashed:
        err = "Crashed";
        break;
    case QProcess::Timedout:
        err = "Time out";
        break;
    case QProcess::ReadError:
        err = "Read error";
        break;
    case QProcess::WriteError:
        err = "Write error";
        break;
    case QProcess::UnknownError:
    default:
        err = "Unknown error";
    }
    return err;
}

BuildGolang::BuildGolang(IApplication *app, QObject *parent) :
    liteApp(app), QObject(parent)
{
    createActions();
    createMenus();
    createToolBars();
    createOutput();

    connect(&buildProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdoutBuild()));
    connect(&buildProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStderrBuild()));
    connect(&buildProcess,SIGNAL(started()),this,SLOT(startedBuild()));
    connect(&buildProcess,SIGNAL(finished(int)),this,SLOT(finishedBuild(int)));
    connect(&buildProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorBuild(QProcess::ProcessError)));

    connect(&runProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdoutRun()));
    connect(&runProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStderrRun()));
    connect(&runProcess,SIGNAL(started()),this,SLOT(startedRun()));
    connect(&runProcess,SIGNAL(finished(int)),this,SLOT(finishedRun(int)));
    connect(&runProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorRun(QProcess::ProcessError)));
}

BuildGolang::~BuildGolang()
{
}

QString BuildGolang::buildName() const
{
    return tr("BuildGoalng");
}

bool BuildGolang::buildProject(IProject *proj)
{
    QString target;
    QStringList val = proj->values("TARGET");
    if (!val.isEmpty())
        target = val.at(0);
    else
        target = proj->displayName();

    target = QFileInfo(target).baseName();

    buildProcess.setWorkingDirectory(QFileInfo(proj->filePath()).absolutePath());

    QString goroot = liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString();

    QStringList args;
    args  << "-ver=false" << "-goroot"<< QDir::fromNativeSeparators(goroot)  << "-gopro" << proj->filePath();
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    buildProcess.start(cmd,args);

    return true;
}

bool BuildGolang::buildFile(const QString &fileName)
{
    QString target = QFileInfo(fileName).baseName();
    QString projDir = QFileInfo(fileName).absolutePath();
    buildProcess.setWorkingDirectory(projDir);

    QString goroot = liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString();

    QStringList args;
    args << "-ver=false" << "-goroot"<< QDir::fromNativeSeparators(goroot)  << "-gofiles" << QFileInfo(fileName).fileName() << "-o" << target;
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    buildProcess.start(cmd,args);

    return false;
}

void BuildGolang::cancelBuild()
{
    if (buildProcess.state() == QProcess::Starting) {
        buildProcess.waitForStarted(3000);
    } else if (buildProcess.state() == buildProcess.Running) {
        buildProcess.waitForFinished(3000);
    }
}

void BuildGolang::appendBuildOutput(const QByteArray &text, bool stdError)
{
    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);

    buildOutputEdit->setCurrentCharFormat(fmt);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    buildOutputEdit->appendPlainText(codec->toUnicode(text));
}

void BuildGolang::appendRunOutput(const QByteArray &text, bool stdError)
{
    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);

    runOutputEdit->setCurrentCharFormat(fmt);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    runOutputEdit->appendPlainText(codec->toUnicode(text));
}


void BuildGolang::finishedBuild(int code)
{
    if (code == 0)
        appendBuildOutput("---- build finish ----",false);
    else
        appendBuildOutput("---- build error ----",false);

    buildProjectAct->setEnabled(true);
    cancelBuildAct->setEnabled(false);
}

void BuildGolang::errorBuild(QProcess::ProcessError code)
{
    appendBuildOutput("Build error : "+processErrorName(code),true);
}

void BuildGolang::readStdoutBuild()
{
    QByteArray text = buildProcess.readAllStandardOutput();
    appendBuildOutput(text,false);
}
void BuildGolang::readStderrBuild()
{
    QByteArray text = buildProcess.readAllStandardError();
    appendBuildOutput(text,true);
}

void BuildGolang::startedBuild()
{
    appendBuildOutput("---- build start ----",false);
    buildProjectAct->setEnabled(false);
    cancelBuildAct->setEnabled(true);
}

void BuildGolang::finishedRun(int code)
{
    stopRunAct->setEnabled(false);
    runAct->setEnabled(true);

    if (code == 0)
        appendRunOutput("---- run finish ----",false);
    else
        appendRunOutput("---- run error ----",false);
}

void BuildGolang::errorRun(QProcess::ProcessError code)
{
    stopRunAct->setEnabled(false);
    runAct->setEnabled(true);

    appendRunOutput("Run error : "+processErrorName(code),true);
}

void BuildGolang::readStdoutRun()
{
    QByteArray text = runProcess.readAllStandardOutput();
    appendRunOutput(text,false);
}
void BuildGolang::readStderrRun()
{
    QByteArray text = runProcess.readAllStandardError();
    appendRunOutput(text,true);
}

void BuildGolang::startedRun()
{
    appendRunOutput("---- run start ----",false);
    stopRunAct->setEnabled(true);
    runAct->setEnabled(false);
    runOutputEdit->setFocus();
}


void BuildGolang::dbclickOutputEdit()
{
    QTextCursor cur = buildOutputEdit->textCursor();
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
    buildOutputEdit->setTextCursor(cur);

    liteApp->mainWindow()->gotoLine(fileName,line,0);
}

void BuildGolang::createActions()
{
    buildProjectAct = new QAction(QIcon(":/images/build.png"),tr("Build Project"),this);
    buildProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    buildProjectAct->setStatusTip(tr("Build Project or File"));
    connect(buildProjectAct,SIGNAL(triggered()),this,SLOT(buildProject()));

    buildFileAct = new QAction(QIcon(":/images/build.png"),tr("Build File\tCtrl+B"),this);
    buildFileAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_B));
    buildFileAct->setStatusTip(tr("Build File"));
    connect(buildFileAct,SIGNAL(triggered()),this,SLOT(buildFile()));

    cancelBuildAct = new QAction(tr("Cancel Build"),this);
    cancelBuildAct->setStatusTip(tr("Cancel Build Project"));
    connect(cancelBuildAct,SIGNAL(triggered()),this,SLOT(cancelBuild()));

    cancelBuildAct->setEnabled(false);

    runAct = new QAction(QIcon(":/images/run.png"),tr("Run"),this);
    runAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runAct->setStatusTip(tr("Run project or file"));    

    connect(runAct, SIGNAL(triggered()),this, SLOT(run()));

    stopRunAct = new QAction(tr("Stop Run"),this);
    stopRunAct->setStatusTip(tr("Stop run"));

    stopRunAct->setEnabled(false);

    connect(stopRunAct, SIGNAL(triggered()),this, SLOT(stopRun()));

}

void BuildGolang::createMenus()
{
    _buildMenu = liteApp->mainWindow()->widget()->menuBar()->addMenu("Build");

    _buildMenu->addSeparator();
    _buildMenu->addAction(buildProjectAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(cancelBuildAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(runAct);
    _buildMenu->addAction(stopRunAct);

    liteApp->mainWindow()->widget()->menuBar()->insertMenu(liteApp->mainWindow()->toolMenu()->menuAction(),_buildMenu);
}

void BuildGolang::createToolBars()
{
    buildToolBar = liteApp->mainWindow()->widget()->addToolBar(tr("Build"));
    buildToolBar->setObjectName("Build");
    buildToolBar->addAction(buildProjectAct);
    buildToolBar->addAction(runAct);
}

void BuildGolang::buildProject()
{
    this->buildOutputEdit->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutputEdit);

    liteApp->mainWindow()->saveAllFile();
    IProject *proj = liteApp->activeProject();
    if (proj)  {
        buildProject(proj);
        return;
    }
    IEditor * edit= liteApp->activeEditor();
    if (edit) {
        buildFile(edit->filePath());
    }
}
void BuildGolang::buildFile()
{
}

void BuildGolang::stopRun()
{
    if (runProcess.state() == QProcess::Starting) {
        runProcess.waitForStarted(100);
    } else if (runProcess.state() == runProcess.Running) {
        runProcess.waitForFinished(100);
    }
    runProcess.kill();
}

void BuildGolang::runOutputKeyEvent(QKeyEvent *e)
{
    if (runProcess.isWritable()) {
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
#ifdef Q_OS_WIN32
            runWriteString += "\r\n";
#else
            runWriteString += "\n";
#endif
            runProcess.write(runWriteString.toUtf8());
            runWriteString.clear();
        } else {
            runWriteString += e->text();
        }
    }
}

void BuildGolang::run()
{
    TargetInfo info = liteApp->getTargetInfo();
    runWriteString.clear();

    if (!info.fileName.isEmpty()) {
        runOutputEdit->clear();
        liteApp->mainWindow()->setCurrentOutputPane(runOutputEdit);
        runProcess.setWorkingDirectory(info.workDir);
        runProcess.start(info.filePath);
    }
}

void BuildGolang::createOutput()
{
    buildOutputEdit = new BuildOutputEdit;
    buildOutputEdit->setReadOnly(true);
    liteApp->mainWindow()->addOutputPane(buildOutputEdit,QIcon(),tr("Build Output"));

    runOutputEdit = new RunOutputEdit;
    runOutputEdit->setReadOnly(false);
    connect(runOutputEdit,SIGNAL(keyEvent(QKeyEvent*)),this,SLOT(runOutputKeyEvent(QKeyEvent*)));
    liteApp->mainWindow()->addOutputPane(runOutputEdit,QIcon(),tr("Run Output"));

    connect(buildOutputEdit,SIGNAL(dbclickEvent()),this,SLOT(dbclickOutputEdit()));
}
