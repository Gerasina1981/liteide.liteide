#ifndef GODOCVIEW_H
#define GODOCVIEW_H

#include <QWidget>
#include <QDockWidget>
#include <QProcess>
#include "../../api/iapp.h"

class QTextBrowser;
class QLineEdit;
class GodocView : public QWidget
{
    Q_OBJECT
public:
    explicit GodocView(IApplication *app, QWidget *parent = 0);
protected:
    IApplication *liteApp;
    QLineEdit     *line;
    QDockWidget   *dock;
    QTextBrowser  *browser;
    QProcess      process;
signals:

public slots:
    void started();
    void readStderr();
    void readStdout();
    void error(QProcess::ProcessError code);
    void finished(int code);
    void packageDoc();
};

#endif // GODOCVIEW_H
