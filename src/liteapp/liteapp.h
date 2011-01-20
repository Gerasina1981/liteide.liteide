#ifndef LITEAPP_H
#define LITEAPP_H

#include "mainwindow.h"
#include "../api/iplugin.h"
#include "../api/iapp.h"
#include "../api/iproject.h"
#include "../api/ibuild.h"
#include "../api/iastview.h"

class LiteApp : public QObject, public IApplication
{
    Q_OBJECT
public:
    LiteApp();
    virtual ~LiteApp();
public:
    void loadAstViewEditor(IEditor *ed);
    void closeProject();
    IBuild *selectBuild(const QString &name);
    virtual QDockWidget * addWorkspacePane(QWidget *w, const QString &name);
    virtual void addOutputPane(QWidget *w, const QString &name);
    virtual void addEditorFactory(IEditorFactory *editFactory);
    virtual void addProjectFactory(IProjectFactory *projFactory);
    virtual void addAstViewFactory(IAstViewFactory *astFactory);
    virtual void addBuild(IBuild *build);
    virtual void gotoLine(const QString &fileName, int line, int col);
    virtual IProject *loadProject(const QString &fileName);
    virtual IEditor  *loadEditor(const QString &fileName);
    virtual IEditorEvent *editorEvent();
    virtual IProjectEvent *projectEvent();
    virtual IBuildEvent *buildEvent();
    virtual IEditor *activeEditor();
    virtual IProject *activeProject();
    virtual IRunTarget *activeRunTarget();
    virtual void setRunTarget(IRunTarget *runTarget);
    virtual IRunTargetEvent *runTargetEvent();
    virtual QSettings *settings();
    virtual QMenu *fileMenu();
    virtual QMenu *editMenu();
    virtual QMenu *viewMenu();
    virtual QMenu *toolMenu();
    virtual QWidget *main();
    virtual QString applicationPath() ;
    virtual QString osExecuteExt();
    void loadPlugins(const QString &dir);
    void installPlugins();
    QString editorTypeFilter() const;
    QString projectTypeFilter() const;
public:
    QSettings  *mainSettings;
    MainWindow * mainWindow;
    QList<IPlugin*> plugins;
    QList<IBuild*>   buildList;
    QList<IEditorFactory*> editorFactorys;
    QList<IProjectFactory*> projectFactorys;
    QList<IAstViewFactory*> astFactorys;
};

#endif // LITEAPP_H
