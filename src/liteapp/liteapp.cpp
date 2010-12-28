#include "liteapp.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>

LiteApp::LiteApp()
{
  mainWindow = new MainWindow(this);
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

