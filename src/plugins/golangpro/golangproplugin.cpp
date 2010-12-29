#include "golangproplugin.h"

#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QPlainTextEdit>
#include <QTextCodec>

GolangProPlugin::GolangProPlugin()
{
}

void GolangProPlugin::createActions()
{
    gofmtAct = new QAction(tr("gofmt\tAlt+F8"),this);
    gofmtAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F8));
    gofmtAct->setStatusTip(tr("Format go file"));

    connect(gofmtAct,SIGNAL(triggered()),this,SLOT(gofmt()));
    liteApp->toolMenu()->addAction(gofmtAct);
}

void GolangProPlugin::createOutput()
{
    gofmtOutputEdit = new QPlainTextEdit;
    gofmtOutputEdit->setReadOnly(true);
    liteApp->addOutputPane(gofmtOutputEdit,tr("gofmt"));
}


void GolangProPlugin::install(IApplication *app)
{
    liteApp = app;
    gofmtCmd = liteApp->settings()->value("golangpro/gofmt","c:\\go\\bin\\gofmt.exe").toString();
    createActions();
    createOutput();
 }

void GolangProPlugin::uninstall()
{

}

QString GolangProPlugin::name() const
{
    return tr("Golang Project");
}

QString GolangProPlugin::info() const
{
    return tr("make by visualfc");
}


Q_EXPORT_PLUGIN(GolangProPlugin)

void GolangProPlugin::gofmt()
{
    IEditor *ed = liteApp->activeEditor();
    if (!ed) {
        return;
    }
    const QString & path = ed->fullPath();
    if (path.isEmpty()) {
        return;
    }
    if (QFileInfo(path).suffix().toLower() != "go") {
        return;
    }

    ed->save();
    connect(&gofmtProcess,SIGNAL(outputText(QString,bool)),gofmtOutputEdit,SLOT(appendPlainText(QString)));
    connect(&gofmtProcess,SIGNAL(processSuccess()),ed,SLOT(reload()));
    gofmtProcess.start(tr("gofmt"),gofmtCmd,
                       QStringList() << "-w=true" << path);
}
