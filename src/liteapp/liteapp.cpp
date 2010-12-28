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

IEditorEvent *LiteApp::editorEvent()
{
    return mainWindow;
}

QSettings *LiteApp::settings()
{
    return mainSettings;
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

QString LiteApp::openFileTypes() const
{
    QStringList types;
    foreach(IEditorFactory *factory, editorFactorys) {
        types.append(factory->openTypes());
    }
    return types.join(";;");
}

IEditor *LiteApp::openFile(const QString &fileName)
{
    QString fileExt = QFileInfo(fileName).suffix();
    foreach(IEditorFactory *factory, editorFactorys) {
        if (factory->fileTypes().contains(fileExt)) {
            IEditor *ed = factory->create(fileName);
            if (ed) {
                return ed;
            }
        }
    }
    return NULL;
}

