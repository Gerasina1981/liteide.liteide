#ifndef _PROCESSOUTPUT_H_
#define _PROCESSOUTPUT_H_

#include <QObject>
#include <QProcess>

class ProcessOutput : public QObject
{
    Q_OBJECT
public:
    ProcessOutput(QObject *parent = 0) : QObject(parent)
    {
        connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStandardOutput()));
        connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStandardError()));
        connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(finished(int,QProcess::ExitStatus)));
        connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));
        connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SIGNAL(processFinished(int,QProcess::ExitStatus)));
        connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SIGNAL(processError(QProcess::ProcessError)));
    }
    void setWorkingDirectory(const QString &dir)
    {
        process.setWorkingDirectory(dir);
    }

    void start(const QString &name, const QString &program, const QStringList &arguments, QProcess::OpenMode mode = QProcess::ReadWrite)
    {
        taskName = name;
        emit outputText(tr("---- %1 started").arg(name),false);
        emit outputText(tr("%1 %2").arg(program).arg(arguments.join(" ")),false);
        process.start(program,arguments,mode);
    }
private:
    QProcess process;
    QString  taskName;
private slots:
    void readStandardError()
    {
        QString text = process.readAllStandardError();
        if (!text.isEmpty()) {
            emit outputText(tr("---- %1 error\n%2").arg(taskName).arg(text),true);
        }
    }
    void readStandardOutput()
    {
        QString text = process.readAllStandardOutput();
        if (!text.isEmpty()) {
            emit outputText(tr("---- %1 output\n%2").arg(taskName).arg(text),false);
        }
    }
    void finished(int code,QProcess::ExitStatus status)
    {
        emit outputText(tr("---- %1 ended, exit code %2").arg(taskName).arg(code),false);
        if (code == 0 && status == QProcess::NormalExit) {
            emit processSuccess();
        }
     }
    void error(QProcess::ProcessError code)
    {
        emit outputText(tr("---- %1 error").arg(taskName),false);
    }
signals:
    void outputText(const QString &text, bool standardError);
    void processFinished(int code, QProcess::ExitStatus status);
    void processError(QProcess::ProcessError code);
    void processSuccess();
};

#endif //_PROCESSOUTPUT_H_
