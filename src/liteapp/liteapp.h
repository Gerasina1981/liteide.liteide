#ifndef LITEAPP_H
#define LITEAPP_H

#include "mainwindow.h"
#include "../api/iplugin.h"
#include "../api/iapp.h"
#include "../api/iproject.h"
#include "../api/ibuild.h"

class LiteApp : public IApplication
{
    Q_OBJECT
public:
    LiteApp();
    virtual ~LiteApp();
public:
    void closeProject();
    virtual IMainWindow *mainWindow();
    virtual void addEditorFactory(IEditorFactory *editFactory);
    virtual void addProjectFactory(IProjectFactory *projFactory);
    virtual IProject *loadProject(const QString &fileName);
    virtual IEditor  *loadEditor(const QString &fileName);
    virtual IEditorEvent *editorEvent();
    virtual IProjectEvent *projectEvent();
    virtual IEditor *activeEditor();
    virtual IProject *activeProject();
    virtual QSettings *settings();
    virtual QString applicationPath() ;
    virtual QString osExecuteExt();
    virtual TargetInfo getTargetInfo();
    virtual QIcon appIcon() const;
    void loadPlugins(const QString &dir);
    void installPlugins();
    QString openTypeFilter() const;
    QString editorTypeFilter() const;
    QString editorNewTypeFilter() const;
    QString projectTypeFilter() const;
public:
    QSettings  *mainSettings;
    MainWindow * _mainWindow;
    QList<IPlugin*> plugins;
    QList<IBuild*>   buildList;
    QList<IEditorFactory*> editorFactorys;
    QList<IProjectFactory*> projectFactorys;
};

#endif // LITEAPP_H
