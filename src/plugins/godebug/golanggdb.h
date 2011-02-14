#ifndef GOLANGGDB_H
#define GOLANGGDB_H

#include <QObject>
#include <QMainWindow>
#include <QProcess>
#include "../../api/iapp.h"
#include "../../api/iproject.h"
#include "../../api/ieditor.h"

class GolangGdb : public QObject
{
    Q_OBJECT
public:
    void createMenus();
    void createActions();
    explicit GolangGdb(IApplication *app, QObject *parent = 0);

signals:

public slots:
protected:
    IApplication *liteApp;
    QMenu  *_debugMenu;
    QAction *debugShellAct;
    QAction *startDebugAct;
    QAction *stopDebugAct;
    QString target;
    QProcess debugProcess;
protected slots:
    void startDebug();
protected slots:
    void stopDebug();
protected slots:
    void debugShell();
};

#endif // GOLANGGDB_H
