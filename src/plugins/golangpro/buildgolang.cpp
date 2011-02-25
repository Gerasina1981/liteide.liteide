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
#include <QProcessEnvironment>
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
static QString processErrorName(QProcess::ProcessError code)
{
    static QString err;
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

    buildProcess = new ProcessEx(this);
    runProcess = new ProcessEx(this);

    connect(buildProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdoutBuild()));
    connect(buildProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStderrBuild()));
    connect(buildProcess,SIGNAL(started()),this,SLOT(startedBuild()));
    connect(buildProcess,SIGNAL(finished(int)),this,SLOT(finishedBuild(int)));
    connect(buildProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorBuild(QProcess::ProcessError)));

    connect(runProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdoutRun()));
    connect(runProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStderrRun()));
    connect(runProcess,SIGNAL(started()),this,SLOT(startedRun()));
    connect(runProcess,SIGNAL(finished(int)),this,SLOT(finishedRun(int)));
    connect(runProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(errorRun(QProcess::ProcessError)));

    connect(liteApp,SIGNAL(activeProjectChanged(IProject*)),this,SLOT(projectChanged(IProject*)));
}

BuildGolang::~BuildGolang()
{
}

QString BuildGolang::buildName() const
{
    return tr("BuildGoalng");
}

QString BuildGolang::goroot()
{
    return QDir::fromNativeSeparators(liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString());
}

QString BuildGolang::gomake()
{
    QString def = goroot()+"/bin/gomake";
    bool usegomake = liteApp->settings()->value("golang/usegomake").toBool();
    if (usegomake) {
        return def;
    }
    return liteApp->settings()->value("golang/make",def).toString();
}

void BuildGolang::buildGoproject(IProject *proj)
{
    bMakefile = false;
    buildProcess->setWorkingDirectory(QFileInfo(proj->filePath()).absolutePath());
    QStringList args;
    args  << "-ver=false" << "-goroot"<< goroot()  << "-gopro" << proj->filePath();
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    buildProcess->setTaskName("Build project");
    buildProcess->start(cmd,args);
}

void BuildGolang::buildFile(const QString &fileName)
{
    bMakefile = false;
    QString target = QFileInfo(fileName).baseName();
    QString projDir = QFileInfo(fileName).absolutePath();
    buildProcess->setWorkingDirectory(projDir);

    QString goroot = liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString();

    QStringList args;
    args << "-ver=false" << "-goroot"<< QDir::fromNativeSeparators(goroot)  << "-gofiles" << QFileInfo(fileName).fileName() << "-o" << target;
    QString cmd = QFileInfo(liteApp->applicationPath(),"gopromake"+liteApp->osExecuteExt()).absoluteFilePath();
    buildProcess->setTaskName("Build file");
    buildProcess->start(cmd,args);
}

void BuildGolang::cancelBuild()
{
    if (buildProcess->state() == QProcess::Starting) {
        buildProcess->waitForStarted(3000);
    } else if (buildProcess->state() == QProcess::Running) {
        buildProcess->waitForFinished(3000);
    }
}

void BuildGolang::appendBuildOutputUTF8(const QByteArray &text, bool stdError)
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

void BuildGolang::appendBuildOutput(const QString &text, bool stdError)
{
    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);

    buildOutputEdit->setCurrentCharFormat(fmt);
     buildOutputEdit->appendPlainText(text);
}

void BuildGolang::appendRunOutputUTF8(const QByteArray &text)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    runOutputEdit->appendPlainText(codec->toUnicode(text));
}

void BuildGolang::appendRunOutput(const QString &text)
{
    runOutputEdit->appendPlainText(text);
}


void BuildGolang::finishedBuild(int code)
{
    QString out = QString("---- %1 exited with code %2").arg(buildProcess->taskName()).arg(code);

    appendBuildOutput(out,bool(code != 0));

    buildProjectAct->setEnabled(true);
    cancelBuildAct->setEnabled(false);
}

void BuildGolang::errorBuild(QProcess::ProcessError code)
{
    QString out = QString("---- %1 error with code %2").arg(buildProcess->taskName()).arg(processErrorName(code));
    appendBuildOutput(out,true);
}

void BuildGolang::readStdoutBuild()
{
    QByteArray text = buildProcess->readAllStandardOutput();
    if (this->bMakefile) {
        appendBuildOutput(QTextCodec::codecForLocale()->toUnicode(text),false);
    } else {
        appendBuildOutputUTF8(text,false);
    }
}
void BuildGolang::readStderrBuild()
{
    QByteArray text = buildProcess->readAllStandardError();
    if (this->bMakefile) {
        appendBuildOutput(QTextCodec::codecForLocale()->toUnicode(text),true);
    } else {
        appendBuildOutputUTF8(text,true);
    }
}

void BuildGolang::startedBuild()
{
    QString out = QString("---- %1 start ...").arg(buildProcess->taskName());
    appendBuildOutput(out,false);
    buildProjectAct->setEnabled(false);
    cancelBuildAct->setEnabled(true);
}

void BuildGolang::finishedRun(int code)
{
    stopRunAct->setEnabled(false);
    runAct->setEnabled(true);

    QString out = QString("---- Program exited with code %1 ").arg(code);

    appendRunOutput(out);
}

void BuildGolang::errorRun(QProcess::ProcessError code)
{
    stopRunAct->setEnabled(false);
    runAct->setEnabled(true);

    QString err = processErrorName(code);
    QString out = QString("---- Error run %1 with code %2").arg(runProcess->programName()).arg(err);

    appendRunOutput(out);
}

void BuildGolang::readStdoutRun()
{
    QByteArray text = runProcess->readAllStandardOutput();
    appendRunOutputUTF8(text);
}
void BuildGolang::readStderrRun()
{
    QByteArray text = runProcess->readAllStandardError();
    appendRunOutputUTF8(text);
}

void BuildGolang::startedRun()
{    
    QString out = QString("---- Starting %1 ...").arg(runProcess->programName());
    appendRunOutput(out);
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

    rebuildProjectAct = new QAction(QIcon(":/images/rebuild.png"),tr("Rebuild Project"),this);
    rebuildProjectAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_B));
    rebuildProjectAct->setStatusTip(tr("Rebuild Project"));
    connect(rebuildProjectAct,SIGNAL(triggered()),this,SLOT(rebuildProject()));

    cancelBuildAct = new QAction(tr("Cancel Build"),this);
    cancelBuildAct->setStatusTip(tr("Cancel Build Project"));
    connect(cancelBuildAct,SIGNAL(triggered()),this,SLOT(cancelBuild()));

    cancelBuildAct->setEnabled(false);

    cleanProjectTempAct = new QAction(QIcon(":/images/cleantmp.png"),tr("Clean Temp Files"),this);
    cleanProjectTempAct->setStatusTip(tr("Clean Temp Files"));
    connect(cleanProjectTempAct,SIGNAL(triggered()),this,SLOT(cleanProjectTemp()));

    cleanProjectAllAct = new QAction(QIcon(":/images/cleanall.png"),tr("Clean All Files"),this);
    cleanProjectAllAct->setStatusTip(tr("Clean All Files"));
    connect(cleanProjectAllAct,SIGNAL(triggered()),this,SLOT(cleanProjectAll()));


    runAct = new QAction(QIcon(":/images/run.png"),tr("Run"),this);
    runAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runAct->setStatusTip(tr("Run project or file"));
    connect(runAct, SIGNAL(triggered()),this, SLOT(run()));

    runGdbAct = new QAction(QIcon(":/images/rungdb.png"),tr("Debug"),this);
    runGdbAct->setShortcut(QKeySequence(Qt::Key_F5));
    runGdbAct->setStatusTip(tr("Debug project"));
    connect(runGdbAct,SIGNAL(triggered()),this,SLOT(runGdb()));

    runShellAct = new QAction(QIcon(":/images/runshell.png"),tr("RunShell"),this);
    runShellAct->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_F5));
    runShellAct->setStatusTip(tr("Run project in shell"));
    connect(runShellAct,SIGNAL(triggered()),this,SLOT(runShell()));


    stopRunAct = new QAction(QIcon(":/images/stoprun.png"),tr("Stop Run"),this);
    stopRunAct->setStatusTip(tr("Stop run"));
    stopRunAct->setEnabled(false);

    connect(stopRunAct, SIGNAL(triggered()),this, SLOT(stopRun()));

}

void BuildGolang::createMenus()
{
    _buildMenu = liteApp->mainWindow()->widget()->menuBar()->addMenu("Build");

    _buildMenu->addSeparator();
    _buildMenu->addAction(buildProjectAct);
    _buildMenu->addAction(rebuildProjectAct);
    _buildMenu->addAction(cancelBuildAct);
    _buildMenu->addAction(cleanProjectTempAct);
    _buildMenu->addAction(cleanProjectAllAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(runAct);
    _buildMenu->addAction(stopRunAct);
    _buildMenu->addAction(runShellAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(runGdbAct);

    liteApp->mainWindow()->widget()->menuBar()->insertMenu(liteApp->mainWindow()->toolMenu()->menuAction(),_buildMenu);
}

void BuildGolang::createToolBars()
{
    buildToolBar = liteApp->mainWindow()->widget()->addToolBar(tr("Build"));
    buildToolBar->setObjectName("Build");
    buildToolBar->addAction(buildProjectAct);
    buildToolBar->addSeparator();
    buildToolBar->addAction(runAct);
    buildToolBar->addAction(stopRunAct);
    buildToolBar->addAction(runShellAct);
    buildToolBar->addSeparator();
    buildToolBar->addAction(runGdbAct);
}

void BuildGolang::rebuildProject()
{
    this->buildOutputEdit->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutputEdit);

    liteApp->mainWindow()->saveAllFile();
    IProject *proj = liteApp->activeProject();
    if (proj)  {
        if (proj->isMakefile()) {
            buildMakefile(proj,true);
        } else {
            buildGoproject(proj);
        }
        return;
    }
    IEditor * edit= liteApp->activeEditor();
    if (edit) {
        buildFile(edit->filePath());
    }
}

void BuildGolang::cleanProjectTemp()
{
    TargetInfo info = liteApp->getTargetInfo();
    if (info.workDir.isEmpty())
        return;

    this->buildOutputEdit->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutputEdit);

    this->buildOutputEdit->appendPlainText("---- Start clean temp");


    if (!info.workDir.isEmpty()) {
        QStringList filter;
        if (liteApp->activeProject()) {
            filter << "*.8" << "*.5" << "*.6";
        } else if (liteApp->activeEditor()) {
            QString base = QFileInfo(info.fileName).baseName();
            filter << base+"_go_.8" << base+"_go_.5" << base+"_go_.6";
        }
        this->RemoveWorkDir(info.workDir,filter);
    }

    this->buildOutputEdit->appendPlainText("---- End clean temp");
}

void BuildGolang::cleanProjectAll()
{
    TargetInfo info = liteApp->getTargetInfo();
    if (info.workDir.isEmpty())
        return;

    this->buildOutputEdit->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutputEdit);

    this->buildOutputEdit->appendPlainText("---- Start clean all");

    if (!info.workDir.isEmpty()) {
        QStringList filter;
        if (liteApp->activeProject()) {
            filter << "*.8" << "*.5" << "*.6";
        } else if (liteApp->activeEditor()) {
            QString base = QFileInfo(info.fileName).baseName();
            filter << base+"_go_.8" << base+"_go_.5" << base+"_go_.6";
        }
        this->RemoveWorkDir(info.workDir,filter);
    }

    if (QFile::exists(info.filePath)) {
        bool b = QFile::remove(info.filePath);
        QString out = QString("remove %1 %2").arg(info.fileName).arg(b?"true":"false");
        this->buildOutputEdit->appendPlainText(out);
    }

    this->buildOutputEdit->appendPlainText("---- End clean all");

}


void BuildGolang::buildProject()
{
    this->buildOutputEdit->clear();
    liteApp->mainWindow()->setCurrentOutputPane(this->buildOutputEdit);

    liteApp->mainWindow()->saveAllFile();
    IProject *proj = liteApp->activeProject();
    if (proj)  {
        if (proj->isMakefile()) {
            buildMakefile(proj,false);
        } else {
            buildGoproject(proj);
        }
        return;
    }
    IEditor * edit= liteApp->activeEditor();
    if (edit) {
        buildFile(edit->filePath());
    }
}

void BuildGolang::buildMakefile(IProject *proj, bool force)
{
    bMakefile = true;
    QString cmd = gomake();
    QStringList args;
    if (force) {
        args << "-B";
    }
    args << "-f" << proj->fileName();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("GOROOT",goroot());

    buildProcess->setEnvironment(env.toStringList());
    buildProcess->setWorkingDirectory(QFileInfo(proj->filePath()).absolutePath());
    if (force) {
        buildProcess->setTaskName("Rebuild Makefile");
    } else {
        buildProcess->setTaskName("Build Makefile");
    }
    buildProcess->start(cmd,args);
}

void BuildGolang::RemoveWorkDir(const QString &workDir, const QStringList &filters)
{
    QDir dir(workDir);
    if (!dir.exists())
        return;

    QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo d, dirs) {
        RemoveWorkDir(d.filePath(),filters);
    }

    QFileInfoList files = dir.entryInfoList(filters,QDir::Files);
    foreach (QFileInfo f, files) {
        bool b = QFile::remove(f.filePath());
        QString out = QString("remove %1 %2").arg(f.fileName()).arg(b?"true":"false");
        buildOutputEdit->appendPlainText(out);
    }
}

void BuildGolang::stopRun()
{
    if (runProcess->state() == QProcess::Starting) {
        runProcess->waitForStarted(100);
    } else if (runProcess->state() == QProcess::Running) {
        runProcess->waitForFinished(100);
    }
    runProcess->kill();
}

void BuildGolang::runOutputKeyEvent(QKeyEvent *e)
{
    if (runProcess->isWritable()) {
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
#ifdef Q_OS_WIN32
            runWriteString += "\r\n";
#else
            runWriteString += "\n";
#endif
            runProcess->write(runWriteString.toUtf8());
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
        runProcess->setWorkingDirectory(info.workDir);
        QStringList args;
        runProcess->start(info.filePath,args);
    }
}

void BuildGolang::runShell()
{
    TargetInfo info = liteApp->getTargetInfo();
    if (!info.fileName.isEmpty()) {
        QStringList args;
        QProcess::startDetached(info.filePath,args,info.workDir);
    }
}

void BuildGolang::runGdb()
{
    TargetInfo info = liteApp->getTargetInfo();
    if (!info.fileName.isEmpty()) {
        QString cmd = liteApp->settings()->value("golang/gdb","gdb").toString();
        QStringList args;
        args << info.filePath;
        QProcess::startDetached(cmd,args,info.workDir);
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

void BuildGolang::projectChanged(IProject *proj)
{
    if (proj == NULL) {
        return;
    }
//    this->rebuildProjectAct->setEnabled(proj->isMakefile());
//    this->cleanProjectAct->setEnabled(proj->isMakefile());
}
