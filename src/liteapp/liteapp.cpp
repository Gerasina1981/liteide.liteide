#include "liteapp.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>

LiteApp::LiteApp()
{
    mainWindow = new MainWindow(this);
    mainSettings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteapp",this);
}

LiteApp::~LiteApp()
{
}

void LiteApp::slotClose()
{
    uninstallPlugins();
    plugins.clear();
}

void LiteApp::addWorkspacePane(QWidget *w, const QString &name)
{
    mainWindow->addWorkspacePane(w,name);
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

void LiteApp::addBuild(IBuild *build)
{
    buildList.append(build);
    mainWindow->appendBuild(build);
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

QSettings *LiteApp::settings()
{
    return mainSettings;
}

QWidget *LiteApp::main()
{
    return mainWindow;
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
}

void LiteApp::uninstallPlugins()
{
    foreach(IPlugin *p, plugins) {
        p->uninstall();
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
    foreach(IEditorFactory *factory, editorFactorys) {
        if (factory->fileTypes().contains(fileExt)) {
            IEditor *ed = factory->create(fileName);
            if (ed) {
                mainWindow->addEditor(ed);
                return ed;
            }
        }
    }
    return NULL;
}

