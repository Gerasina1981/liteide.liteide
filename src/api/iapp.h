#ifndef __LITEAPI_IAPP_H__
#define __LITEAPI_IAPP_H__

#include <QWidget>
#include <QDockWidget>
#include <QSettings>
#include <QMenu>

class IEditorFactory;
class IEditorEvent;
class IEditor;
class IProject;
class IProjectFactory;
class IProjectEvent;
class IMainWindow;
class IOption;

class IMainWindow
{
public:
    virtual ~IMainWindow() {}
    virtual QMainWindow *widget() = 0;
    virtual QDockWidget * addWorkspacePane(QWidget *w, const QString &name) = 0;
    virtual void addOutputPane(QWidget *w, const QIcon &icon, const QString &name) = 0;
    virtual void setCurrentOutputPane(QWidget *w) = 0;
    virtual void addOptionPage(IOption *opt) = 0;
    virtual QMenu *fileMenu() = 0;
    virtual QMenu *viewMenu() = 0;
    virtual QMenu *editMenu() = 0;
    virtual QMenu *toolMenu() = 0;
    virtual void closeAllFile() = 0;
    virtual void saveAllFile() = 0;
    virtual void gotoLine(const QString &fileName, int line, int col) = 0;
};

struct TargetInfo
{
    QString workDir;
    QString fileName;
    QString filePath;
};

class IApplication : public QObject
{
    Q_OBJECT
public:
    virtual ~IApplication() {}
    virtual void addEditorFactory(IEditorFactory *editFactory) = 0;
    virtual void addProjectFactory(IProjectFactory *projFactory) = 0;
    virtual IProject *loadProject(const QString &fileName) = 0;
    virtual IEditor *loadEditor(const QString &fileName) = 0;
    virtual IEditorEvent *editorEvent() = 0;
    virtual IProjectEvent *projectEvent() = 0;
    virtual QSettings *settings() = 0;
    virtual IMainWindow *mainWindow() = 0;
    virtual IEditor *activeEditor() = 0;
    virtual IProject *activeProject() = 0;
    virtual QString applicationPath() = 0;
    virtual QString osExecuteExt() = 0;
    virtual TargetInfo getTargetInfo() = 0;
signals:
    void activeEditorTextChanged(IEditor *ed);
    void activeEditorChanged(IEditor *ed);
    void activeProjectChanged(IProject *proj);
    void closeEditor(IEditor *ed);
public:
    void emitActiveEditorTextChanged(IEditor *ed)
    {
        emit activeEditorTextChanged(ed);
    }
    void emitActiveEditorChanged(IEditor *ed)
    {
        emit activeEditorChanged(ed);
    }
    void emitCloseEditor(IEditor *ed)
    {
        emit closeEditor(ed);
    }
};

#endif //__LITEAPI_IAPP_H__
