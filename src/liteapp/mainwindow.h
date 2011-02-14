#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMap>
#include "../api/ieditor.h"
#include "../api/iproject.h"
#include "../api/iapp.h"
#include <QTimer>

class LiteApp;
class QPlainTextEdit;
class QActionGroup;
class FindDialog;
class QStackedWidget;

class MainWindow : public QMainWindow,
        public IMainWindow,
        public IEditorEvent,
        public IProjectEvent
{
    Q_OBJECT
public:
    friend class LiteApp;
    virtual void gotoLine(const QString &fileName, int line, int col);
    void loadSettings();
    MainWindow(LiteApp *app);
    virtual QMainWindow *widget();
    virtual QMenu *fileMenu();
    virtual QMenu *viewMenu();
    virtual QMenu *editMenu();
    virtual QMenu *toolMenu();
    virtual void fireDocumentChanged(IEditor *edit, bool b);
    virtual void fireDocumentSave(IEditor *edit);
    virtual void fireTextChanged(IEditor *edit);
    virtual void fireProjectChanged(IProject *project);
    virtual void fireProjectClose(IProject *project);
protected:
    virtual void closeEvent(QCloseEvent *event);
private slots:
    void selectedOutputAct(QAction *act);
    void findText(const QString&,QTextDocument::FindFlags);
    void find();
    void closeAllFile();
    void saveAllFile();
    void outputTabChanged(int index);
    void newProject();
    void closeProject();
    void saveFile();
    void newFile();
    void openFile();
    void openProject();
    void about();
    void editTabChanged(int index);
    void editTabClose(int index);
    void aboutPlugins();
public:
    virtual QDockWidget * addWorkspacePane(QWidget *w, const QString &name);
    virtual void addOutputPane(QWidget *w, const QIcon &icon, const QString &name);
    virtual void setCurrentOutputPane(QWidget *w);
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

    QTabWidget *editTabWidget;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QToolBar *outputToolBar;
    QStackedWidget *outputStackedWidget;
    QActionGroup *outputActGroup;
    QAction *lastOutputAct;
    QMap<QAction*,QWidget*> outputActMap;

    QMenu   *_fileMenu;
    QMenu   *_editMenu;
    QMenu   *_viewMenu;
    QMenu   *_toolMenu;
    QMenu   *_helpMenu;

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

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;
    QAction *aboutPluginsAct;

    FindDialog *findDialog;

    IEditor *activeEditor;
    IProject *activeProject;
    LiteApp *liteApp;
    QMap<QWidget*,IEditor*> editors; 
};

#endif // MAINWINDOW_H
