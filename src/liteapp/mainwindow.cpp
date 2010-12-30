#include "mainwindow.h"
#include "liteapp.h"
#include "aboutpluginsdialog.h"

#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QCloseEvent>
#include <QDebug>

MainWindow::MainWindow(LiteApp *app) : liteApp(app), activeEditor(NULL)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    resize(640,480);

    editTabWidget = new QTabWidget;
    editTabWidget->setDocumentMode(true);
    editTabWidget->setTabsClosable(true);

    setCentralWidget(editTabWidget);

    connect(editTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editTabChanged(int)));
    connect(editTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editTabClose(int)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    createOutputWidget();

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int count = editTabWidget->count();
    while (count--) {
        QWidget *w = editTabWidget->widget(count);
        IEditor *ed = editors.value(w,NULL);
        if (ed) {
            if (ed->close()) {
                editTabWidget->removeTab(count);
            } else {
                event->ignore();
                return;
            }
        }
    }
    event->accept();
}

void MainWindow::createActions()
{
    openProjectAct = new QAction(QIcon(":/images/open.png"), tr("&Open Project"),this);
    openProjectAct->setShortcuts(QKeySequence::Open);
    openProjectAct->setStatusTip(tr("Open Project"));
    connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

    newFileAct = new QAction(QIcon(":/images/new.png"), tr("&New File"),this);
    newFileAct->setShortcuts(QKeySequence::New);
    newFileAct->setStatusTip(tr("Create a new file"));
    connect(newFileAct, SIGNAL(triggered()), this, SLOT(newFile()));

    saveFileAct = new QAction(QIcon(":/images/save.png"), tr("&Save..."), this);
    saveFileAct->setShortcuts(QKeySequence::Save);
    saveFileAct->setStatusTip(tr("Save file"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    openFileAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open file"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last editing action"));
    //connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last editing action"));
    //connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    buildProjectAct = new QAction(tr("Build Project\tCtrl+B"),this);
    buildProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    buildProjectAct->setStatusTip(tr("Build Project"));

    runAct = new QAction(tr("Run"),this);
    runAct->setStatusTip(tr("Run project or file"));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutPluginsAct = new QAction(tr("About Plugins..."),this);
    aboutPluginsAct->setStatusTip(tr("Show the plugins"));
    connect(aboutPluginsAct,SIGNAL(triggered()),this, SLOT(aboutPlugins()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));

    fileMenu->addAction(openProjectAct);
    fileMenu->addSeparator();

    fileMenu->addAction(newFileAct);
    fileMenu->addAction(saveFileAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);


    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    buildMenu = menuBar()->addMenu(tr("&Build"));
    buildMenu->addAction(buildProjectAct);
    buildMenu->addSeparator();
    buildMenu->addAction(runAct);

    toolMenu = menuBar()->addMenu(tr("&Tools"));

    menuBar()->addSeparator();


    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    helpMenu->addAction(aboutPluginsAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newFileAct);
    fileToolBar->addAction(saveFileAct);
    fileToolBar->addAction(openFileAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::addWorkspacePane(QWidget *w, const QString &name)
{
    QDockWidget *dock = new QDockWidget(name, this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(w);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
}

void MainWindow::editTabClose(int index)
{
    QWidget *w = editTabWidget->widget(index);
    if (!w)
        return;
    QMap<QWidget*,IEditor*>::const_iterator it = editors.find(w);
    if (it == editors.end()) {
        return;
    }
    IEditor *ed = it.value();
    if (!ed) {
        return;
    }
    if (ed->close()) {
        editTabWidget->removeTab(index);
        editors.remove(w);
    }
}

void MainWindow::editTabChanged(int index)
{
    QWidget *w = editTabWidget->widget(index);
    if (!w) {
        return;
    }
    QMap<QWidget*,IEditor*>::const_iterator it = editors.find(w);
    if (it == editors.end()) {
        return;
    }
    IEditor *ed = it.value();
    if (!ed) {
        return;
    }
    activeEditor = ed;

    activeEditor->activeEditor(undoAct,redoAct);
    //connect(this,SLOT(undo()),ed,SLOT(undo()));
}

void MainWindow::addEditor(IEditor *editor)
{
    editors.insert(editor->widget(),editor);
    int index = editTabWidget->addTab(editor->widget(),editor->icon(),editor->fileName());
    editTabWidget->setCurrentIndex(index);
}

void MainWindow::addOutputPage(QWidget *w, const QString &name)
{
    outputTabWidget->addTab(w,name);
}

void MainWindow::createDockWindows()
{
    this->setDockOptions(ForceTabbedDocks);
}

void MainWindow::createOutputWidget()
{
    outputTabWidget = new QTabWidget;
    QDockWidget *dock = new QDockWidget(tr("Output"), this);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetClosable);
    dock->setWidget(outputTabWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());

}

void MainWindow::saveFile()
{
    if (activeEditor) {
        activeEditor->save();
    }
}

void MainWindow::newFile()
{

}

void MainWindow::openProject()
{
    static QString path;
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this,
           tr("Open Project"), path, liteApp->projectTypeFilter());

    if (!fileName.isEmpty()) {
        path = QFileInfo(fileName).absolutePath();
        liteApp->loadProject(fileName);
    }
}

void MainWindow::openFile()
{
    static QString path;
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this,
           tr("Open File"), path, liteApp->editorTypeFilter());

    if (!fileName.isEmpty()) {
        path = QFileInfo(fileName).absolutePath();
        liteApp->loadEditor(fileName);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About LiteIDE"),
                tr("The <b>LiteIDE</b> lite program editor. "
                   "<p>Create by <i>visualfc@gmail.com</i>"));
}

void MainWindow::fireDocumentChanged(IEditor *edit, bool b)
{
    int index = editTabWidget->indexOf(edit->widget());
    if (index >= 0) {
        editTabWidget->setTabText(index, b ? edit->fileName()+"[*]" : edit->fileName());
    }
}

void MainWindow::fireProjectChanged(IProject *project)
{
    activeProject = project;
}

void MainWindow::aboutPlugins()
{
    AboutPluginsDialog dlg(this);
    dlg.resize(400,300);
    foreach (IPlugin *p, liteApp->plugins) {
        dlg.addPluginInfo(p->name(),p->info());
    }

    dlg.exec();
}
