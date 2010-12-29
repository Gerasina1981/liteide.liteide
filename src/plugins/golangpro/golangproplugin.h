#ifndef GOLANGPROPLUGIN_H
#define GOLANGPROPLUGIN_H

#include "golangpro_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/ieditor.h"
#include "../../util/processoutput.h"

#include <QObject>
#include <QtPlugin>
#include <QPlainTextEdit>


class GOLANGPROSHARED_EXPORT GolangProPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    GolangProPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString info() const;
    void createActions();
    void createOutput();
private slots:
    void gofmt();
protected:
    IApplication *liteApp;
    ProcessOutput gofmtProcess;
    IEditor *curEditor;
    QPlainTextEdit *gofmtOutputEdit;
    QAction *gofmtAct;
    QString gofmtCmd;
    QString go8gCmd;
    QString go8lCmd;
};

#endif // GOLANGPROPLUGIN_H
