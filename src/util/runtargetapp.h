#ifndef RUNTARGETAPP_H
#define RUNTARGETAPP_H

#include "../api/iapp.h"
#include "../api/iruntarget.h"

#include <QObject>
#include <QProcess>

class RunTargetApp : public QObject, public IRunTarget
{
    Q_OBJECT
public:
    RunTargetApp(IApplication *app, QObject *parent = 0);
    virtual void runProject(IProject *proj);
    virtual void runEditor(IEditor *edit);
private slots:
    void started();
    void readStderr();
    void readStdout();
    void error(QProcess::ProcessError code);
    void finished(int code);
private:
    IApplication *liteApp;
    QProcess process;
};


#endif // RUNTARGETAPP_H
