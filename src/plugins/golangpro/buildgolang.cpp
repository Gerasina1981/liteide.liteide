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


BuildGolang::BuildGolang(IApplication *app, QObject *parent) :
    liteApp(app), QObject(parent)
{
    createActions();
    createMenus();
    createToolBars();

    runApp = new RunTargetApp(app);


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
    if (runApp) {
        delete runApp;
    }
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


    runTargetAct = new QAction(QIcon(":/images/run.png"),tr("Run"),this);
    runTargetAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    runTargetAct->setStatusTip(tr("Run project or file"));
    connect(runTargetAct, SIGNAL(triggered()),this, SLOT(runTarget()));
}

void BuildGolang::createMenus()
{
    _buildMenu = liteApp->mainWindow()->widget()->menuBar()->addMenu("Build");

    _buildMenu->addSeparator();
    _buildMenu->addAction(buildProjectAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(cancelBuildAct);
    _buildMenu->addSeparator();
    _buildMenu->addAction(runTargetAct);

    liteApp->mainWindow()->widget()->menuBar()->insertMenu(liteApp->mainWindow()->toolMenu()->menuAction(),_buildMenu);
}

void BuildGolang::createToolBars()
{
    buildToolBar = liteApp->mainWindow()->widget()->addToolBar(tr("Build"));
    buildToolBar->setObjectName("Build");
    buildToolBar->addAction(buildProjectAct);
    buildToolBar->addAction(runTargetAct);
}

void BuildGolang::buildProject()
{
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

void BuildGolang::runTarget()
{
    IProject *proj = liteApp->activeProject();
    if (proj) {
        runApp->runProject(proj);
        return;
    }
    IEditor *edit = liteApp->activeEditor();
    if (edit) {
        runApp->runEditor(edit);
    }
}
