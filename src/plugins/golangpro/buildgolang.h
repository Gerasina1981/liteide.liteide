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
    virtual bool buildProject(IProject *proj);
    virtual bool buildFile(const QString &fileName);

    void appendBuildOutput(const QByteArray &text, bool stdError);
    void appendRunOutput(const QByteArray &text, bool stdError);
public:
    QProcess        buildProcess;
    QProcess        runProcess;
    IApplication    *liteApp;
    BuildOutputEdit *buildOutputEdit;
    RunOutputEdit   *runOutputEdit;
    QAction *buildProjectAct;
    QAction *buildFileAct;
    QAction *cancelBuildAct;
    QAction *runAct;
    QAction *stopRunAct;
    QAction *debugAct;
    QToolBar *buildToolBar;
    QMenu *_buildMenu;
    QString runWriteString;
private slots:
    void runOutputKeyEvent(QKeyEvent*);
    void run();
    void stopRun();
    void cancelBuild();
    void buildFile();
    void buildProject();
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
