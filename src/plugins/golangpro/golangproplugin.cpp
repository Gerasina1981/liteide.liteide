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
    build = new BuildGolang(app);
    app->addBuild(build);

    createGofmt();
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
    return tr("Golang Project 1.0");
}

void GolangProPlugin::createGofmt()
{
    gofmtOutputEdit = new QPlainTextEdit;
    gofmtOutputEdit->setReadOnly(true);
    liteApp->addOutputPane(gofmtOutputEdit,tr("gofmt"));

    gofmtAct = new QAction(tr("gofmt\tAlt+F8"),this);
    gofmtAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F8));
    gofmtAct->setStatusTip(tr("Format go file"));

    connect(gofmtAct,SIGNAL(triggered()),this,SLOT(gofmt()));
    liteApp->toolMenu()->addAction(gofmtAct);
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

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString goroot = env.value("GOROOT");
    if (goroot.isEmpty()) {
#ifdef Q_OS_WIN32
        goroot = "c:/go";
#else
        goroot = env.value("HOME")+"/go";
#endif
    }
    QString projDir = QFileInfo(path).absolutePath();
    gofmtProcess.setWorkingDirectory(projDir);

    QString cmd = goroot+"/bin/gofmt"+liteApp->osExecuteExt();

    gofmtProcess.start("gofmt",cmd,
                       QStringList() << "-w=true" << ed->fileName());
}

Q_EXPORT_PLUGIN(GolangProPlugin)
