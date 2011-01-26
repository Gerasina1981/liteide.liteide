#include "liteapp.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QApplication>

LiteApp::LiteApp()
{
    mainSettings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteapp",this);
    mainWindow = new MainWindow(this);
}

LiteApp::~LiteApp()
{
    qDeleteAll(plugins);
}

QDockWidget * LiteApp::addWorkspacePane(QWidget *w, const QString &name)
{
    return mainWindow->addWorkspacePane(w,name);
}

void LiteApp::addOutputPane(QWidget *w, const QString &name)
{
    mainWindow->addOutputPage(w,name);
}

void LiteApp::addEditorFactory(IEditorFactory *editFactory)
{
    editorFactorys.append(editFactory);
}

void LiteApp::addProjectFactory(IProjectFactory *projFactory)
{
    projectFactorys.append(projFactory);
}

void LiteApp::addAstViewFactory(IAstViewFactory *astFactory)
{
    astFactorys.append(astFactory);
}

void LiteApp::addBuild(IBuild *build)
{
    buildList.append(build);
    mainWindow->appendBuild(build);
}

void LiteApp::gotoLine(const QString &fileName, int line, int col)
{
    mainWindow->gotoLine(fileName,line,col);
}

IProject *LiteApp::loadProject(const QString &fileName)
{
    QString ext = QFileInfo(fileName).suffix();
    foreach(IProjectFactory *factory, projectFactorys) {
        if (factory->projectKeys().contains(ext)) {
            return factory->loadProject(fileName);
        }
    }
    return NULL;
}

IEditorEvent *LiteApp::editorEvent()
{
    return mainWindow;
}

IProjectEvent *LiteApp::projectEvent()
{
    return mainWindow;
}

IBuildEvent *LiteApp::buildEvent()
{
    return mainWindow;
}

QSettings *LiteApp::settings()
{
    return mainSettings;
}

QWidget *LiteApp::main()
{
    return mainWindow;
}

QString LiteApp::applicationPath()
{
    return qApp->applicationDirPath();
}

QString LiteApp::osExecuteExt()
{
#ifdef Q_OS_WIN32
    return ".exe";
#else
    return "";
#endif
}

QMenu *LiteApp::fileMenu()
{
    return mainWindow->fileMenu;
}

QMenu *LiteApp::editMenu()
{
    return mainWindow->editMenu;
}

QMenu *LiteApp::viewMenu()
{
    return mainWindow->viewMenu;
}

QMenu *LiteApp::toolMenu()
{
    return mainWindow->toolMenu;
}

IEditor *LiteApp::activeEditor()
{
    return mainWindow->activeEditor;
}

IProject *LiteApp::activeProject()
{
    return mainWindow->activeProject;
}

IRunTarget *LiteApp::activeRunTarget()
{
    return mainWindow->activeRunTarget;
}

void LiteApp::setRunTarget(IRunTarget *runTarget)
{
    mainWindow->activeRunTarget = runTarget;
}

IRunTargetEvent *LiteApp::runTargetEvent()
{
    return mainWindow;
}


void LiteApp::loadPlugins(const QString &dir)
{
    QDir pluginsDir = dir;
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        if (IPlugin *p = qobject_cast<IPlugin*>(loader.instance())) {
            plugins.append(p);
        }
    }
}

void LiteApp::installPlugins()
{
    foreach(IPlugin *p, plugins) {
        p->install(this);
    }
    if (!buildList.empty()) {
        mainWindow->activeBuild = buildList.at(0);
        mainWindow->activeBuild->setActive();
    }
}

QString LiteApp::projectTypeFilter() const
{
    QStringList types;
    foreach(IProjectFactory *factory, projectFactorys) {
        types.append(factory->projectTypeFilter());
    }
    return types.join(";;");
}

QString LiteApp::editorTypeFilter() const
{
    QStringList types;
    foreach(IEditorFactory *factory, editorFactorys) {
        types.append(factory->editorTypeFilter());
    }
    return types.join(";;");
}

IEditor *LiteApp::loadEditor(const QString &fileName)
{
    foreach (IEditor *ed, mainWindow->editors.values()) {
        if (ed->filePath() == fileName) {
            mainWindow->editTabWidget->setCurrentWidget(ed->widget());
            return ed;
        }
    }

    QString fileExt = QFileInfo(fileName).suffix();
    if (editorFactorys.isEmpty()) {
        return NULL;
    }
    IEditorFactory *factory = editorFactorys.at(0);
    QListIterator<IEditorFactory*> i(editorFactorys);
    while (i.hasNext()) {
        IEditorFactory *v = i.next();
        if (v->fileTypes().contains(fileExt)) {
            factory = v;
            break;
        }
    }
    IEditor *ed = factory->create(fileName);
    if (ed) {
        mainWindow->addEditor(ed);
        return ed;
    }
    return NULL;
}


IBuild *LiteApp::selectBuild(const QString &name)
{
    foreach(IBuild *build, buildList) {
        if (build->buildName() == name) {
            build->setActive();
            return build;
        }
    }
    return NULL;
}

void LiteApp::closeProject()
{
    if (mainWindow->activeProject)
        mainWindow->activeProject->close();
    mainWindow->activeProject = NULL;
}

void LiteApp::loadAstViewEditor(const IEditor *ed)
{   
    if (ed == NULL) {
        foreach (IAstViewFactory *factory, astFactorys) {
           factory->load("","");
        }
        return;
    }
    QString ext = QFileInfo(ed->filePath()).suffix();
    foreach (IAstViewFactory *factory, astFactorys) {
        if (factory->fileTypes().contains(ext)) {
            factory->load(ed->filePath(),ed->data());
        } else {
            factory->load("","");
        }
    }
}
