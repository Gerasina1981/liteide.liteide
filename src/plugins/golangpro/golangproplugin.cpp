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
    createActions();
    createOutput();
 }

void GolangProPlugin::uninstall()
{

}

QString GolangProPlugin::name() const
{
    return "GolangPro";
}

QString GolangProPlugin::anchor() const
{
    return "visualfc";
}

QString GolangProPlugin::info() const
{
    return tr("Golang tools 1.0");
}


Q_EXPORT_PLUGIN(GolangProPlugin)

void GolangProPlugin::gofmt()
{
    IEditor *ed = liteApp->activeEditor();
    if (!ed) {
        return;
    }
    const QString & path = ed->filePath();
    if (path.isEmpty()) {
        return;
    }
    if (QFileInfo(path).suffix().toLower() != "go") {
        return;
    }

    ed->save();
    gofmtProcess.disconnect();
    connect(&gofmtProcess,SIGNAL(outputText(QString,bool)),gofmtOutputEdit,SLOT(appendPlainText(QString)));
    connect(&gofmtProcess,SIGNAL(processSuccess()),ed,SLOT(reload()));
    gofmtProcess.start(tr("gofmt"),"gofmt",
                       QStringList() << "-w=true" << path);
}
