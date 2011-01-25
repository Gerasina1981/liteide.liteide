#include "goastview.h"

#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QFileInfo>

GoAstView::GoAstView(IApplication *app, QObject *parent):
    liteApp(app), QObject(parent)
{
    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&process,SIGNAL(started()),this,SLOT(started()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));

}

void GoAstView::update(const QString &fileName, const QByteArray &data)
{
    if (process.state() == QProcess::Running)
        return;

    QString projDir = QFileInfo(fileName).absolutePath();
    process.setWorkingDirectory(projDir);

    QStringList args;
    args << "-src" << fileName;
    QString cmd = QFileInfo(liteApp->applicationPath(),"goastview"+liteApp->osExecuteExt()).absoluteFilePath();
    process.start(cmd,args);
}

void GoAstView::started()
{
}

void GoAstView::readStderr()
{
    QByteArray data = process.readAllStandardError();
}

void GoAstView::readStdout()
{
    QByteArray data = process.readAllStandardOutput();
    emit astOutput(data);
}

void GoAstView::error(QProcess::ProcessError code)
{
    emit astOutput("");
}

void GoAstView::finished(int code)
{
}
