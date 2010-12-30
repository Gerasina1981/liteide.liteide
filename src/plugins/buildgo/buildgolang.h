#ifndef BUILDGOLANG_H
#define BUILDGOLANG_H

#include <QObject>
#include <QProcess>
#include "../../api/ibuild.h"
#include "../../api/iproject.h"
#include "../../api/ieditor.h"
#include "../../api/iapp.h"
#include "../../api/ioutput.h"
#include "../../util/runtargetapp.h"

class BuildGolang : public QObject, public IBuild
{
    Q_OBJECT
public:
    explicit BuildGolang(IApplication *app, QObject *parent = 0);
    virtual ~BuildGolang();
    virtual QString buildName() const;
    virtual void setActive();
    virtual bool buildProject(IProject *proj);
    virtual bool buildEditor(IEditor *edit);
    virtual void cancelBuild();
public:
    QProcess        process;
    QString         target;
    bool            build8g;
    bool            build8l;
    IApplication    *liteApp;
    RunTargetApp    *runTarget;
private slots:
    void started();
    void readStderr();
    void readStdout();
    void error(QProcess::ProcessError code);
    void finished(int code);
};

#endif // BUILDGOLANG_H
