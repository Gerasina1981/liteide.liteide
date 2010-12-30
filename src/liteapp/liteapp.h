#ifndef LITEAPP_H
#define LITEAPP_H

#include "mainwindow.h"
#include "../api/iplugin.h"
#include "../api/iapp.h"
#include "../api/iproject.h"

class LiteApp : public QObject, public IApplication
{
    Q_OBJECT

public:
    LiteApp();
    ~LiteApp();
    virtual void addWorkspacePane(QWidget *w, const QString &name);
    virtual void addOutputPane(QWidget *w, const QString &name);
    virtual void addEditorFactory(IEditorFactory *editFactory);
    virtual void addProjectFactory(IProjectFactory *projFactory);
    virtual IProject *loadProject(const QString &fileName);
    virtual IEditor  *loadEditor(const QString &fileName);
    virtual IEditorEvent *editorEvent();
    virtual IProjectEvent *projectEvent();
    virtual IEditor *activeEditor();
    virtual IProject *activeProject();
    virtual QSettings *settings();
    virtual QMenu *fileMenu();
    virtual QMenu *editMenu();
    virtual QMenu *viewMenu();
    virtual QMenu *toolMenu();
    virtual QWidget *main();
    void loadPlugins(const QString &dir);
    void installPlugins();
    void uninstallPlugins();
    QString editorTypeFilter() const;
    QString projectTypeFilter() const;
public:
    QSettings  *mainSettings;
    MainWindow * mainWindow;
    QList<IPlugin*> plugins;
    QList<IEditorFactory*> editorFactorys;
    QList<IProjectFactory*> projectFactorys;
private slots:
    void slotClose();
};

#endif // LITEAPP_H
