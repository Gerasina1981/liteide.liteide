#ifndef GOLANGPROPLUGIN_H
#define GOLANGPROPLUGIN_H

#include "golangpro_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/ieditor.h"
#include "../../util/processoutput.h"
#include "buildgolang.h"
#include "golangoption.h"

#include <QObject>
#include <QtPlugin>
#include <QPlainTextEdit>


class /*GOLANGPROSHARED_EXPORT*/ GolangProPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    void createGofmt();
    GolangProPlugin();
    virtual ~GolangProPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual const PluginInfo & info() const;
private:
    IApplication *liteApp;
    BuildGolang *build;
    ProcessOutput gofmtProcess;
    GolangOption *opt;
    QPlainTextEdit *gofmtOutputEdit;
    QAction *gofmtAct;
private slots:
    void gofmt();
};

#endif // GOLANGPROPLUGIN_H
