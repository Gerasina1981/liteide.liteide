#ifndef GOASTVIEW_H
#define GOASTVIEW_H

#include <QObject>
#include "../../api/iapp.h"
#include <QProcess>

class GoAstView : public QObject
{
    Q_OBJECT
public:
    explicit GoAstView(IApplication *app,QObject *parent = 0);
    virtual ~GoAstView();
    virtual void update(const QString &fileName, const QByteArray &data);
    QProcess astProcess;
signals:
    void astOutput(const QByteArray &data);
    void astError(const QByteArray &data);
public slots:
    void started();
    void readStderr();
    void readStdout();
    void error(QProcess::ProcessError code);
    void finished(int code);
public:
    IApplication *liteApp;
    QByteArray  srcData;
};

#endif // GOASTVIEW_H
