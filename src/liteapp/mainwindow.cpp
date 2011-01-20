#include "mainwindow.h"
#include "liteapp.h"
#include "aboutpluginsdialog.h"
#include "finddialog.h"
#include "../util/texteditoutput.h"
#include "projectwizard.h"

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
#include <QTextBlock>
#include <QRegExp>
#include <QFileInfo>

MainWindow::MainWindow(LiteApp *app) :
        liteApp(app),
        activeEditor(NULL),
        activeProject(NULL),
        activeBuild(NULL),
        activeRunTarget(NULL),
        findDialog(NULL)
{
    this->setWindowTitle("LiteIDE");
    this->setAttribute(Qt::WA_DeleteOnClose);

    editTabWidget = new QTabWidget;
    editTabWidget->setDocumentMode(true);
    editTabWidget->setTabsClosable(true);


    setCentralWidget(editTabWidget);
    editTabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

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
    loadSettings();
    outputDock->hide();
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
    liteApp->settings()->setValue("geometry",saveGeometry());
    liteApp->settings()->setValue("state",saveState());

    event->accept();
}

void MainWindow::createActions()
{
    newProjectAct = new QAction(tr("&New Project"),this);
    newProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_N));
    newProjectAct->setStatusTip(tr("Open Project"));
    connect(newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

    openProjectAct = new QAction(tr("&Open Project"),this);
    openProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_O));
    openProjectAct->setStatusTip(tr("Open Project"));
    connect(openProjectAct, SIGNAL(triggered()), this, SLOT(openProject()));

    closeProjectAct = new QAction(tr("&Close Project"),this);
    closeProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_C));
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

    saveAllFileAct = new QAction(tr("Save All"), this);
    saveAllFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    saveAllFileAct->setStatusTip(tr("Save All Files"));
    connect(saveAllFileAct, SIGNAL(triggered()), this, SLOT(saveAllFile()));

    closeAllFileAct = new QAction(tr("Close All"), this);
    closeAllFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_W));
    closeAllFileAct->setStatusTip(tr("Close All Files"));
    connect(closeAllFileAct, SIGNAL(triggered()), this, SLOT(closeAllFile()));

    openFileAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open file"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));


    findAct = new QAction(tr("&Find"), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setStatusTip(tr("Find Text"));
    connect(findAct,SIGNAL(triggered()),this,SLOT(find()));

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo the last editing action"));
    //connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo the last editing action"));
    //connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    buildActGroup = new QActionGroup(this);

    buildProjectAct = new QAction(QIcon(":/images/build.png"),tr("Build Project"),this);
    buildProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    buildProjectAct->setStatusTip(tr("Build Project or File"));
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
    runTargetAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
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

    fileMenu->addAction(newProjectAct);
    fileMenu->addAction(openProjectAct);
    fileMenu->addAction(closeProjectAct);
    fileMenu->addSeparator();

    fileMenu->addAction(newFileAct);
    fileMenu->addAction(openFileAct);
    fileMenu->addAction(saveFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAllFileAct);
    fileMenu->addAction(closeAllFileAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);


    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);

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
    fileToolBar->setObjectName("File");
    fileToolBar->addAction(newFileAct);
    fileToolBar->addAction(saveFileAct);
    fileToolBar->addAction(openFileAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("Edit");
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);

    buildToolBar = addToolBar(tr("Build"));
    buildToolBar->setObjectName("Build");
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
    dock->setObjectName(name);
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
    liteApp->loadAstViewEditor(activeEditor);
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
    //this->setDockOptions(ForceTabbedDocks);
}

void MainWindow::createOutputWidget()
{
    outputDock = new QDockWidget(tr("Output"), this);
    outputDock->setObjectName("outputDock");

    outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    outputDock->setFeatures(QDockWidget::DockWidgetClosable);

    //outputDock->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    viewMenu->addAction(outputDock->toggleViewAction());
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);

    outputTabWidget = new QTabWidget;
    outputTabWidget->setTabPosition(QTabWidget::South);

    buildOutputEdit = new PlainTextEditEx(this);
    buildOutputEdit->setReadOnly(true);

    connect(buildOutputEdit,SIGNAL(dbclickEvent()),this,SLOT(dbclickOutputEdit()));

    outputTabWidget->addTab(buildOutputEdit,tr("Build Output"));

    runTargetOutputEdit = new QPlainTextEdit(this);
    runTargetOutputEdit->setReadOnly(true);
    outputTabWidget->addTab(runTargetOutputEdit,tr("Application Output"));

    outputDock->setWidget(outputTabWidget);

    connect(outputTabWidget,SIGNAL(currentChanged(int)),this,SLOT(outputTabChanged(int)));
}

void MainWindow::saveFile()
{
    if (activeEditor) {
        activeEditor->save();
    }
}

void MainWindow::newFile()
{
    static QString path = liteApp->applicationPath()+"/..";

    if (activeProject) {
        path = QFileInfo(this->activeProject->filePath()).absolutePath();
    }

    QString fileName;

    fileName = QFileDialog::getSaveFileName(this,
           tr("Save New File"), path, liteApp->editorTypeFilter());

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            if (QFileInfo(fileName).suffix() == "go") {
                file.write("package main");
            }
            file.close();
            path = QFileInfo(fileName).absolutePath();
            liteApp->loadEditor(fileName);
        }
    }
}

void MainWindow::openProject()
{
    static QString path = liteApp->applicationPath()+"/..";

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
    static QString path = liteApp->applicationPath()+"/..";
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
                QString(tr("<h1 class='title'>LiteIDE </h1>"
                   "<p>Open Source Editor - Copyright (C)2011</p>"
                   "<p></p>"
                   "<table border=0>"
                   "<tr><td>Devoloper:    </td><td>visualfc (visualfc@gmail.com)</td></tr>"
                   "<tr><td>Project:</td><td>http://code.google.com/p/liteide</td></tr>"
                   "<tr><td>Version:</td><td>0.1.7</td></tr>"
                   "<tr><td>Build Time:</td><td>%1</td></tr>"
                   "</table>").arg(__DATE__)));
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
    if (activeEditor && activeEditor == edit) {
        liteApp->loadAstViewEditor(activeEditor);
    }
}

void MainWindow::fireProjectClose(IProject *project)
{
    activeProject = NULL;
    setWindowTitle(tr("LiteIDE"));
    closeAllFile();
}

void MainWindow::fireProjectChanged(IProject *project)
{
    activeProject = project;
    if (activeProject) {
        setWindowTitle(QString(tr("LiteIDE - %1")).arg(activeProject->displayName()));
    } else {
        setWindowTitle(tr("LiteIDE"));
    }
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
    dlg.resize(450,300);
    foreach (IPlugin *p, liteApp->plugins) {
        dlg.addPluginInfo(p->name(),p->anchor(),p->info());
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

    saveAllFile();

    buildOutputEdit->moveCursor(QTextCursor::End);

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

    runTargetOutputEdit->moveCursor(QTextCursor::End);

    if (activeProject) {
        activeRunTarget->runProject(activeProject);
    } else if (activeEditor) {
        activeRunTarget->runEditor(activeEditor);
    }
}

void MainWindow::saveAllFile()
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

void MainWindow::newProject()
{
    ProjectWizard wiz(this);
    static QString path = liteApp->applicationPath()+"/..";
    QString location = liteApp->settings()->value("PROJECT_LOCATION",path).toString();
    wiz.setField("PROJECT_LOCATION",QDir::toNativeSeparators(location));
    if (wiz.exec() == QDialog::Accepted) {
        QString location = wiz.field("PROJECT_LOCATION").toString();
        liteApp->settings()->setValue("PROJECT_LOCATION",location);
        QMessageBox::StandardButton ret;
        ret = QMessageBox::question(this, tr("LiteIDE"),
                                    tr("Project '%1' is created.\n"
                                       "Do you want to load?")
                                    .arg(QFileInfo(wiz.projectFileName).baseName()),
                                    QMessageBox::Yes | QMessageBox::No
                                    | QMessageBox::Cancel);
        if (ret == QMessageBox::Yes) {
            liteApp->loadProject(wiz.projectFileName);
        }
    }
}

void MainWindow::outputTabChanged(int index)
{
//    QString text = outputTabWidget->tabText(index);
//    outputDock->setWindowTitle(QString(tr("Output - %1")).arg(text));
}

void MainWindow::closeAllFile()
{
    int count = editTabWidget->count();
    while (count--) {
        QWidget *w = editTabWidget->widget(count);
        IEditor *ed = editors.value(w,NULL);
        if (ed && ed->close()) {
            editTabWidget->removeTab(count);
            editors.remove(w);
        }
    }
}

void MainWindow::find()
{
    if (!activeEditor)
        return;

    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog,SIGNAL(findText(QString,QTextDocument::FindFlags)),this,SLOT(findText(QString,QTextDocument::FindFlags)));
    }
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MainWindow::findText(const QString& text,QTextDocument::FindFlags flags)
{
    if (activeEditor) {
        QPlainTextEdit*ed = qobject_cast<QPlainTextEdit*>(activeEditor->widget());
        if (ed) {
            ed->find(text,flags);
        }
    }
}

void MainWindow::dbclickOutputEdit()
{
    QTextCursor cur = buildOutputEdit->textCursor();
    QRegExp rep("(\\w+[\\\\/])*(\\w+[.]\\w+)+:(\\d+)");
    int index = rep.indexIn(cur.block().text());
    if (index < 0)
        return;
    QStringList list = rep.capturedTexts();
    if (list.count() < 3)
        return;

    QString cap = list.at(0);
    QStringList capList = cap.split(":");
    if (capList.count() < 2) {
        return;
    }
    QString fileName = capList[0];
    QString fileLine = capList[1];
    bool ok = false;
    int line = fileLine.toInt(&ok);
    if (!ok)
        return;

    cur.select(QTextCursor::LineUnderCursor);
    buildOutputEdit->setTextCursor(cur);

    this->gotoLine(fileName,line,0);
}

void MainWindow::loadSettings()
{
    restoreGeometry(liteApp->settings()->value("geometry").toByteArray());
    restoreState(liteApp->settings()->value("state").toByteArray());
}

void MainWindow::gotoLine(const QString &fileName, int line, int col)
{
    IEditor *ed = NULL;
    if (activeProject) {
        QString projectPath = QFileInfo(activeProject->filePath()).absolutePath();
        QString filePath = QFileInfo(projectPath,fileName).absoluteFilePath();
        ed = liteApp->loadEditor(filePath);
    } else if (activeEditor && activeEditor->fileName() == fileName) {
        ed = activeEditor;
    }
    if (!ed)
        return;
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(ed->widget());
    if (!editor)
        return;

    int count = 1;
    for ( QTextBlock b = editor->document()->begin(); b.isValid(); b = b.next(), count++ )
    {
        if ( count == line )
        {
            QTextCursor c = QTextCursor(b);
            editor->setFocus();
            editor->setTextCursor(c);
            break;
        }
    }
}
