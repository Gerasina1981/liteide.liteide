#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMap>
#include "../api/ieditor.h"
#include "../api/iproject.h"
#include "../api/ibuild.h"
#include "../api/iruntarget.h"

class LiteApp;
class QPlainTextEdit;
class QActionGroup;
class FindDialog;

class PlainTextEditEx : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit PlainTextEditEx(QWidget *parent = 0) :
            QPlainTextEdit(parent)
    {
    }
    virtual void mouseDoubleClickEvent(QMouseEvent *e)
    {
        QPlainTextEdit::mouseDoubleClickEvent(e);
        emit dbclickEvent();
    }
signals:
    void dbclickEvent();
};

class MainWindow : public QMainWindow,
        public IEditorEvent,
        public IProjectEvent,
        public IBuildEvent,
        public IRunTargetEvent
{
    Q_OBJECT
public:
    friend class LiteApp;
    void appendBuild(IBuild *build);
    MainWindow(LiteApp *app);
    virtual void fireDocumentChanged(IEditor *edit, bool b);
    virtual void fireDocumentSave(IEditor *edit);
    virtual void fireProjectChanged(IProject *project);
    virtual void fireProjectClose(IProject *project);
    virtual void fireBuildStarted();
    virtual void fireBuildStoped(bool success);
    virtual void fireBuildOutput(const QString &text, bool stdError);
    virtual void fireRunTargetStarted();
    virtual void fireRunTargetStoped(bool success);
    virtual void fireRunTargetOutput(const QByteArray &text, bool stdError);
protected:
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void dbclickOutputEdit();
    void findText(const QString&,QTextDocument::FindFlags);
    void find();
    void closeAllFile();
    void saveAllFile();
    void outputTabChanged(int index);
    void newProject();
    void closeProject();
    void buildFile();
    void runTarget();
    void cancelBuild();
    void buildProject();
    void saveFile();
    void newFile();
    void openFile();
    void openProject();
    void about();
    void editTabChanged(int index);
    void editTabClose(int index);
    void aboutPlugins();
public:
    QDockWidget * addWorkspacePane(QWidget *w, const QString &name);
    void addOutputPage(QWidget *w, const QString &name);
    void addEditor(IEditor *ed);
private:
    virtual void dropEvent(QDropEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void createOutputWidget();

    QDockWidget *outputDock;
    QTabWidget *editTabWidget;
    QTabWidget *outputTabWidget;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *buildToolBar;

    QMenu   *fileMenu;
    QMenu   *editMenu;
    QMenu   *viewMenu;
    QMenu   *buildMenu;
    QMenu   *buildListMenu;
    QMenu   *toolMenu;
    QMenu   *helpMenu;

    QAction *newProjectAct;
    QAction *openProjectAct;
    QAction *closeProjectAct;
    QAction *newFileAct;
    QAction *openFileAct;
    QAction *saveFileAct;
    QAction *closeFileAct;
    QAction *saveAllFileAct;
    QAction *closeAllFileAct;

    QAction *findAct;

    QAction *undoAct;
    QAction *redoAct;

    QActionGroup *buildActGroup;
    QAction *buildProjectAct;
    QAction *buildFileAct;
    QAction *cancelBuildAct;
    QAction *runTargetAct;
    QAction *debugAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;
    QAction *aboutPluginsAct;

    PlainTextEditEx   *buildOutputEdit;
    QPlainTextEdit   *runTargetOutputEdit;

    FindDialog *findDialog;

    IEditor *activeEditor;
    IProject *activeProject;
    IBuild   *activeBuild;
    IRunTarget *activeRunTarget;
    LiteApp *liteApp;
    QMap<QWidget*,IEditor*> editors;
public slots:
    void selectBuild();
};

#endif // MAINWINDOW_H
