#include "goastview.h"

#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include "../../util/processutil.h"

GoAstView::GoAstView(IApplication *app, QObject *parent):
    liteApp(app), QObject(parent)
{
    connect(&astProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&astProcess,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&astProcess,SIGNAL(started()),this,SLOT(started()));
    connect(&astProcess,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&astProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
}

GoAstView::~GoAstView()
{
    astProcess.waitForFinished(100);
}

void GoAstView::update(const QString &fileName, const QByteArray &data)
{
    if (astProcess.state() == QProcess::Running)
        return;

    QString projDir = QFileInfo(fileName).absolutePath();
    astProcess.setWorkingDirectory(projDir);

    srcData = data;
    QStringList args;
    args << "-src" << fileName;
    if (!data.isEmpty()) {
        args << "-stdin=true";
    }
    QString cmd = QFileInfo(liteApp->applicationPath(),"goastview"+liteApp->osExecuteExt()).absoluteFilePath();
    astProcess.start(cmd,args); 
}

void GoAstView::started()
{
    if (!srcData.isEmpty()) {
        astProcess.write(srcData);
        astProcess.closeWriteChannel();
    }
}

void GoAstView::readStderr()
{
    QByteArray data = astProcess.readAllStandardError();
    emit astError(data);
}

void GoAstView::readStdout()
{
    QByteArray data = astProcess.readAllStandardOutput();
    emit astOutput(data);
}

void GoAstView::error(QProcess::ProcessError code)
{
    QByteArray data = processErrorName(code);
    emit astError(data);
}

void GoAstView::finished(int /*code*/)
{
}
