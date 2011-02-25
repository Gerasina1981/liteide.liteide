#include "golangproplugin.h"

#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QPlainTextEdit>
#include <QTextCodec>

GolangProPlugin::GolangProPlugin() : build(NULL)
{
}

GolangProPlugin::~GolangProPlugin()
{
    if (build) {
        delete build;
    }
}


void GolangProPlugin::install(IApplication *app)
{
    liteApp = app;
    opt = new GolangOption(liteApp);
    liteApp->mainWindow()->addOptionPage(opt);
    build = new BuildGolang(app);

    createGofmt();
}

void GolangProPlugin::uninstall()
{

}

const PluginInfo & GolangProPlugin::info() const
{
    static PluginInfo s;
    s.name = "golangpro";
    s.anchor = "visualfc";
    s.info = "Golang Project";
    s.ver = "1.0";
    return s;
}

void GolangProPlugin::createGofmt()
{
    gofmtOutputEdit = new QPlainTextEdit;
    gofmtOutputEdit->setReadOnly(true);
    liteApp->mainWindow()->addOutputPane(gofmtOutputEdit,QIcon(),tr("Gofmt Output"));

    gofmtAct = new QAction(tr("Gofmt"),this);
    gofmtAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT + Qt::Key_I));
    gofmtAct->setStatusTip(tr("Format go file"));

    connect(gofmtAct,SIGNAL(triggered()),this,SLOT(gofmt()));
    liteApp->mainWindow()->toolMenu()->addAction(gofmtAct);
}

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

    QString goroot = build->goroot();
    QString projDir = QFileInfo(path).absolutePath();
    gofmtProcess.setWorkingDirectory(projDir);

    QString cmd = goroot+"/bin/gofmt"+liteApp->osExecuteExt();

    gofmtProcess.start("gofmt",cmd,
                       QStringList() << "-w=true" << ed->fileName());
}

Q_EXPORT_PLUGIN(GolangProPlugin)
