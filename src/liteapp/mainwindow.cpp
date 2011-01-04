#include "mainwindow.h"
#include "liteapp.h"
#include "aboutpluginsdialog.h"
#include "../util/texteditoutput.h"

#include <QApplication>
#include <QAction>
#include <QActionGroup>
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
#include <QVBoxLayout>
#include <QTextEdit>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QPlainTextEdit>
#include <QUrl>

MainWindow::MainWindow(LiteApp *app) :
        liteApp(app),
        activeEditor(NULL),
        activeProject(NULL),
        activeBuild(NULL),
        activeRunTarget(NULL)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    editTabWidget = new QTabWidget;
    editTabWidget->setDocumentMode(true);
    editTabWidget->setTabsClosable(true);


    setCentralWidget(editTabWidget);
    editTabWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    connect(editTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editTabChanged(int)));
    connect(editTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editTabClose(int)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    createOutputWidget();

    resize(640,480);

    setAcceptDrops(true);
    //setUnifiedTitleAndToolBarOnMac(true);
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

    closeProjectAct = new QAction(QIcon(":/images/close.png"), tr("&Close Project"),this);
    //closeProjectAct->setShortcuts(QKeySequence::Open);
    closeProjectAct->setStatusTip(tr("Close Project"));
    connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()));


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

    buildActGroup = new QActionGroup(this);

    buildProjectAct = new QAction(QIcon(":/images/build.png"),tr("Build Project\tCtrl+B"),this);
    buildProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    buildProjectAct->setStatusTip(tr("Build Project"));
    connect(buildProjectAct,SIGNAL(triggered()),this,SLOT(buildProject()));

    buildFileAct = new QAction(QIcon(":/images/build.png"),tr("Build File\tCtrl+B"),this);
    buildFileAct->setShortcut(QKeySequence(Qt::ALT + Qt::Key_B));
    buildFileAct->setStatusTip(tr("Build File"));
    connect(buildFileAct,SIGNAL(triggered()),this,SLOT(buildFile()));

    cancelBuildAct = new QAction(tr("Cancel Build"),this);
 //   cancelBuildAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    cancelBuildAct->setStatusTip(tr("Cancel Build Project"));
    connect(cancelBuildAct,SIGNAL(triggered()),this,SLOT(cancelBuild()));


    runTargetAct = new QAction(QIcon(":/images/run.png"),tr("Run"),this);
    runTargetAct->setStatusTip(tr("Run project or file"));
    connect(runTargetAct, SIGNAL(triggered()),this, SLOT(runTarget()));

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
    fileMenu->addAction(closeProjectAct);
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
    buildListMenu = buildMenu->addMenu("&Select Build");

    buildMenu->addSeparator();
    buildMenu->addAction(buildProjectAct);
    buildMenu->addSeparator();
    buildMenu->addAction(cancelBuildAct);
    buildMenu->addSeparator();
    buildMenu->addAction(runTargetAct);

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

    buildToolBar = addToolBar(tr("Build"));
    buildToolBar->addAction(buildProjectAct);
    buildToolBar->addAction(runTargetAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

QDockWidget *MainWindow::addWorkspacePane(QWidget *w, const QString &name)
{
    QDockWidget *dock = new QDockWidget(name, this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setWidget(w);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    viewMenu->addAction(dock->toggleViewAction());
    return dock;
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
    outputDock = new QDockWidget(tr("Output"), this);
    outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    outputDock->setFeatures(QDockWidget::DockWidgetClosable);
    viewMenu->addAction(outputDock->toggleViewAction());
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);

    outputTabWidget = new QTabWidget;
    outputTabWidget->setTabPosition(QTabWidget::South);

    buildOutputEdit = new QPlainTextEdit(this);
    outputTabWidget->addTab(buildOutputEdit,tr("Build Output"));

    runTargetOutputEdit = new QPlainTextEdit(this);
    outputTabWidget->addTab(runTargetOutputEdit,tr("Application Output"));

    outputDock->setWidget(outputTabWidget);
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

void MainWindow::fireDocumentSave(IEditor *edit)
{
    if (activeProject && edit->filePath() == activeProject->filePath()) {
         activeProject->reload();
    }
}

void MainWindow::fireProjectChanged(IProject *project)
{
    activeProject = project;
}

void MainWindow::fireBuildStarted()
{
    buildProjectAct->setEnabled(false);
    cancelBuildAct->setEnabled(true);
}

void MainWindow::fireBuildStoped(bool success)
{
    buildProjectAct->setEnabled(true);
    cancelBuildAct->setEnabled(false);
    fireBuildOutput(QString("build exit %1 !")
                    .arg(success? tr("normal") : tr("error")),
                    !success);
}

void MainWindow::fireBuildOutput(const QString &text, bool stdError)
{
    if (outputDock->isHidden()) {
        outputDock->show();
    }
    if (outputTabWidget->currentWidget() != buildOutputEdit)
        outputTabWidget->setCurrentWidget(buildOutputEdit);

    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);
    buildOutputEdit->setCurrentCharFormat(fmt);
    buildOutputEdit->appendPlainText(text);
}

void MainWindow::fireRunTargetStarted()
{

}

void MainWindow::fireRunTargetStoped(bool success)
{

}

void MainWindow::fireRunTargetOutput(const QByteArray &text, bool stdError)
{
    if (outputDock->isHidden()) {
        outputDock->show();
    }
    if (outputTabWidget->currentWidget() != runTargetOutputEdit)
        outputTabWidget->setCurrentWidget(runTargetOutputEdit);

    QTextCharFormat fmt;
    if (stdError)
        fmt.setForeground(Qt::red);
    else
        fmt.setForeground(Qt::black);

    //QTextCodec::fromUnicode()
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    runTargetOutputEdit->setCurrentCharFormat(fmt);
    runTargetOutputEdit->appendPlainText(codec->toUnicode(text));
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

void MainWindow::appendBuild(IBuild *build)
{
    QAction *act = buildActGroup->addAction(build->buildName());
    connect(act,SIGNAL(triggered()),this,SLOT(selectBuild()));
    act->setCheckable(true);
    buildListMenu->addAction(act);
}

void MainWindow::selectBuild()
{
    QAction *act = buildActGroup->checkedAction();
    activeBuild = liteApp->selectBuild(act->text());
}

void MainWindow::buildProject()
{
    if (!activeBuild) {
        return;
    }

    saveAll();

    if (activeProject) {
        activeBuild->buildProject(activeProject);
    } else if(activeEditor) {
        activeBuild->buildFile(activeEditor->filePath());
    }
}

void MainWindow::cancelBuild()
{
    if (!activeBuild) {
        return;
    }
    activeBuild->cancelBuild();
}

void MainWindow::runTarget()
{
    if (!activeRunTarget) {
        return;
    }
    if (activeProject) {
        activeRunTarget->runProject(activeProject);
    } else if (activeEditor) {
        activeRunTarget->runEditor(activeEditor);
    }
}

void MainWindow::saveAll()
{
    foreach(IEditor *ed, editors) {
        ed->save();
    }
}

void MainWindow::buildFile()
{

}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
    /*
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
        return;
    //qDebug() << fileName;
    QString ext = QFileInfo(fileName).suffix();

    //event->acceptProposedAction();
    */
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;
    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
        return;

    if (!liteApp->loadProject(fileName)) {
        liteApp->loadEditor(fileName);
    }
}

void MainWindow::closeProject()
{
    if (activeProject) {
        activeProject->close();
    }
}
