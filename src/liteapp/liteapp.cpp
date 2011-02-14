#include "liteapp.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QApplication>

LiteApp::LiteApp()
{
    mainSettings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"liteide","liteapp",this);
    _mainWindow = new MainWindow(this);
}

LiteApp::~LiteApp()
{
    qDeleteAll(plugins);
}

IMainWindow *LiteApp::mainWindow()
{
    return _mainWindow;
}

void LiteApp::addEditorFactory(IEditorFactory *editFactory)
{
    editorFactorys.append(editFactory);
}

void LiteApp::addProjectFactory(IProjectFactory *projFactory)
{
    projectFactorys.append(projFactory);
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
    return _mainWindow;
}

IProjectEvent *LiteApp::projectEvent()
{
    return _mainWindow;
}

QSettings *LiteApp::settings()
{
    return mainSettings;
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

IEditor *LiteApp::activeEditor()
{
    return _mainWindow->activeEditor;
}

IProject *LiteApp::activeProject()
{
    return _mainWindow->activeProject;
}

void LiteApp::loadPlugins(const QString &dir)
{
    QDir pluginsDir = dir;
    pluginsDir.setFilter(QDir::Files | QDir::NoSymLinks);
    foreach (QString fileName, pluginsDir.entryList()) {
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
    foreach (IEditor *ed, _mainWindow->editors.values()) {
        if (ed->filePath() == fileName) {
            _mainWindow->editTabWidget->setCurrentWidget(ed->widget());
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
        _mainWindow->addEditor(ed);
        return ed;
    }
    return NULL;
}


void LiteApp::closeProject()
{
    if (_mainWindow->activeProject)
        _mainWindow->activeProject->close();
    _mainWindow->activeProject = NULL;
}
