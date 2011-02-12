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
#include "../../util/runtargetapp.h"

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


class BuildGolang : public QObject, public IBuild
{
    Q_OBJECT
public:
    void createToolBars();
    void createMenus();
    void createActions();
    explicit BuildGolang(IApplication *app, QObject *parent = 0);
    virtual ~BuildGolang();
    virtual QString buildName() const;
    virtual bool buildProject(IProject *proj);
    virtual bool buildFile(const QString &fileName);

    void appendBuildOutput(const QString &text, bool stdError);
public:
    QProcess        process;
    QString         target;
    IApplication    *liteApp;
    RunTargetApp    *runApp;
    BuildOutputEdit *buildOutput;
    QAction *buildProjectAct;
    QAction *buildFileAct;
    QAction *cancelBuildAct;
    QAction *runTargetAct;
    QAction *debugAct;
    QToolBar *buildToolBar;
private slots:
    void runTarget();
    void cancelBuild();
    void buildFile();
    void buildProject();
    void started();
    void readStderr();
    void readStdout();
    void error(QProcess::ProcessError code);
    void finished(int code);
public slots:
    void dbclickOutputEdit();
};

#endif // BUILDGOLANG_H
