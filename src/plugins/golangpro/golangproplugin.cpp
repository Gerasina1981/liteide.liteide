#include "golangproplugin.h"

#include <QProcess>
#include <QFileInfo>
#include <QDebug>

GolangProPlugin::GolangProPlugin()
{
    gofmtAct = new QAction(tr("gofmt\tAlt+F8"),this);
    gofmtAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F8));
    gofmtAct->setStatusTip(tr("Format go file"));

    connect(gofmtAct,SIGNAL(triggered()),this,SLOT(gofmt()));
}

void GolangProPlugin::install(IApplication *app)
{
    liteApp = app;
    liteApp->toolMenu()->addAction(gofmtAct);
    gofmtCmd = liteApp->settings()->value("golangpro/gofmt","c:\\go\\bin\\gofmt.exe").toString();
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

    int code = QProcess::execute(gofmtCmd, QStringList() << "-w=true" << path);
    if (code == 0)
        ed->reload();
}
