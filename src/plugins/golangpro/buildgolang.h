#ifndef BUILDGOLANG_H
#define BUILDGOLANG_H

#include <QObject>
#include <QProcess>
#include <QPlainTextEdit>
#include <QMainWindow>

#include "../../api/ibuild.h"
#include "../../api/iproject.h"
#include "../../api/ieditor.h"
#include "../../api/iapp.h"
#include "../../api/ioutput.h"

class ProcessEx : public QProcess
{
public:
    ProcessEx(QObject *parent) : QProcess(parent)
    {
    }
public:
    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite)
    {
        _programName = program;
        QProcess::start(program,arguments,mode);
    }
    void start(const QString &program, OpenMode mode = ReadWrite)
    {
        _programName = program;
        QProcess::start(program,mode);
    }

    void setTaskName(const QString &name)
    {
        _taskName = name;
    }
    QString taskName() const
    {
        return _taskName;
    }
    QString programName() const
    {
        return _programName;
    }
protected:
    QString _programName;
    QString _taskName;
};

class BuildOutputEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit BuildOutputEdit(QWidget *parent = 0) :
            QPlainTextEdit(parent)
    {
    }
    virtual void mouseDoubleClickEvent(QMouseEvent *e)
    {
        QPlainTextEdit::mouseDoubleClickEvent(e);
        emit dbclickEvent();
    }
signals:
    void dbclickEvent();
};

class RunOutputEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit RunOutputEdit(QWidget *parent = 0) :
            QPlainTextEdit(parent)
    {
    }
    virtual void keyPressEvent(QKeyEvent *e)
    {
        emit keyEvent(e);
        if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
            e->ignore();
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
    }
signals:
    void keyEvent(QKeyEvent*);
};

class BuildGolang : public QObject//, public IBuild
{
    Q_OBJECT
public:
    void createOutput();
    void createToolBars();
    void createMenus();
    void createActions();
    explicit BuildGolang(IApplication *app, QObject *parent = 0);
    virtual ~BuildGolang();
    virtual QString buildName() const;
    QString goroot();
    QString gomake();
    void buildGoproject(IProject *proj);
    void buildMakefile(IProject *proj, bool force);
    void buildFile(const QString &fileName);
    void RemoveWorkDir(const QString &wordDir, const QStringList &filters);
    void appendBuildOutputUTF8(const QByteArray &text, bool stdError);
    void appendBuildOutput(const QString &text, bool stdError);
    void appendRunOutputUTF8(const QByteArray &text);
    void appendRunOutput(const QString &text);
public:
    bool            bMakefile;
    ProcessEx       *buildProcess;
    ProcessEx       *runProcess;
    IApplication    *liteApp;
    BuildOutputEdit *buildOutputEdit;
    RunOutputEdit   *runOutputEdit;
    QAction *buildProjectAct;
    QAction *rebuildProjectAct;
    QAction *cancelBuildAct;
    QAction *runAct;
    QAction *runGdbAct;
    QAction *runShellAct;
    QAction *stopRunAct;
    QAction *debugAct;
    QAction *cleanProjectTempAct;
    QAction *cleanProjectAllAct;
    QToolBar *buildToolBar;
    QMenu *_buildMenu;
    QString runWriteString;
private slots:
    void projectChanged(IProject*);
    void runOutputKeyEvent(QKeyEvent*);
    void run();
    void runShell();
    void runGdb();
    void stopRun();
    void cancelBuild();
    void buildProject();
    void rebuildProject();
    void cleanProjectTemp();
    void cleanProjectAll();
    void startedBuild();
    void readStderrBuild();
    void readStdoutBuild();
    void errorBuild(QProcess::ProcessError code);
    void finishedBuild(int code);
    void startedRun();
    void readStderrRun();
    void readStdoutRun();
    void errorRun(QProcess::ProcessError code);
    void finishedRun(int code);
public slots:
    void dbclickOutputEdit();
};

#endif // BUILDGOLANG_H
