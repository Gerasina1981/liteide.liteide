#include "mainwindow.h"
#include "liteapp.h"
#include "aboutpluginsdialog.h"
#include "finddialog.h"
#include "projectwizard.h"
#include "optionsdialog.h"
#include "aboutdialog.h"

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
#include <QSplitter>
#include <QStackedWidget>
#include <QDebug>

MainWindow::MainWindow(LiteApp *app) :
        liteApp(app),
        activeEditor(NULL),
        activeProject(NULL),
        findDialog(NULL),
        lastOutputAct(NULL)
{
    this->setWindowTitle("LiteIDE");
    this->setWindowIcon(liteApp->appIcon());
    this->setAttribute(Qt::WA_DeleteOnClose);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical,this);

    outputStackedWidget = new QStackedWidget(this);

    editTabWidget = new QTabWidget;
    editTabWidget->setDocumentMode(true);
    editTabWidget->setTabsClosable(true);

    editTabWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    mainSplitter->addWidget(editTabWidget);
    mainSplitter->addWidget(outputStackedWidget);

    mainSplitter->setStretchFactor(0,50);

    outputStackedWidget->hide();

    setCentralWidget(mainSplitter);


    connect(editTabWidget,SIGNAL(currentChanged(int)),this,SLOT(editTabChanged(int)));
    connect(editTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(editTabClose(int)));

    createActions();
    createMenus();
    createToolBars();
    //createStatusBar();
    createOutputWidget();  

    resize(640,480);

    setAcceptDrops(true);

    //setUnifiedTitleAndToolBarOnMac(true);
    optDialog = new OptionsDialog(this);
    optDialog->setModal(true);

    loadSettings();
}

QMainWindow *MainWindow::widget()
{
    return this;
}

QMenu *MainWindow::fileMenu()
{
    return _fileMenu;
}

QMenu *MainWindow::viewMenu()
{
    return _viewMenu;
}

QMenu *MainWindow::editMenu()
{
    return _editMenu;
}

QMenu *MainWindow::toolMenu()
{
    return _toolMenu;
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
    newProjectAct = new QAction(QIcon(":/images/newproj.png"),tr("New Project..."),this);
    newProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_N));
    newProjectAct->setStatusTip(tr("New Project"));
    connect(newProjectAct, SIGNAL(triggered()), this, SLOT(newProject()));

    closeProjectAct = new QAction(tr("Close Project"),this);
    //closeProjectAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));
    closeProjectAct->setStatusTip(tr("Close Project"));
    connect(closeProjectAct, SIGNAL(triggered()), this, SLOT(closeProject()));


    newFileAct = new QAction(QIcon(":/images/new.png"), tr("New File..."),this);
    newFileAct->setShortcuts(QKeySequence::New);
    newFileAct->setStatusTip(tr("Create a new file"));
    connect(newFileAct, SIGNAL(triggered()), this, SLOT(newFile()));

    saveFileAct = new QAction(QIcon(":/images/save.png"), tr("&Save File"), this);
    saveFileAct->setShortcuts(QKeySequence::Save);
    saveFileAct->setStatusTip(tr("Save file"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    saveFileAsAct = new QAction(tr("&Save File As..."), this);
    saveFileAsAct->setShortcuts(QKeySequence::SaveAs);
    saveFileAsAct->setStatusTip(tr("Save File As"));
    connect(saveFileAsAct, SIGNAL(triggered()), this, SLOT(saveFileAs()));


    saveAllFileAct = new QAction(tr("Save All"), this);
    saveAllFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    saveAllFileAct->setStatusTip(tr("Save All Files"));
    connect(saveAllFileAct, SIGNAL(triggered()), this, SLOT(saveAllFile()));

    closeFileAct = new QAction(tr("Close File"), this);
    closeFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    closeFileAct->setStatusTip(tr("Close Active File"));
    connect(closeFileAct, SIGNAL(triggered()), this, SLOT(closeFile()));

    closeAllFileAct = new QAction(tr("Close All"), this);
    closeAllFileAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_W));
    closeAllFileAct->setStatusTip(tr("Close All Files"));
    connect(closeAllFileAct, SIGNAL(triggered()), this, SLOT(closeAllFile()));

    openFileAct = new QAction(QIcon(":/images/open.png"), tr("&Open File or Project..."), this);
    openFileAct->setShortcuts(QKeySequence::Open);
    openFileAct->setStatusTip(tr("Open File or Project"));
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

    optionsAct = new QAction(QIcon(),tr("&Options..."),this);
    optionsAct->setStatusTip(tr("Show options dialog"));
    connect(optionsAct,SIGNAL(triggered()),this,SLOT(options()));

    quitAct = new QAction(tr("Quit"), this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutPluginsAct = new QAction(tr("About Plugins..."),this);
    aboutPluginsAct->setStatusTip(tr("Show the plugins"));
    connect(aboutPluginsAct,SIGNAL(triggered()),this, SLOT(aboutPlugins()));
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));

    _fileMenu->addAction(newProjectAct);
    _fileMenu->addAction(newFileAct);
    _fileMenu->addAction(openFileAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(closeProjectAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(saveFileAct);
    _fileMenu->addAction(saveFileAsAct);
    _fileMenu->addAction(saveAllFileAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(closeFileAct);
    _fileMenu->addAction(closeAllFileAct);
    _fileMenu->addSeparator();
    _fileMenu->addAction(quitAct);


    _editMenu = menuBar()->addMenu(tr("&Edit"));
    _editMenu->addAction(undoAct);
    _editMenu->addAction(redoAct);
    _editMenu->addSeparator();
    _editMenu->addAction(findAct);

    _viewMenu = menuBar()->addMenu(tr("&View"));

    _toolMenu = menuBar()->addMenu(tr("&Tools"));
    _toolMenu->addAction(optionsAct);
    _toolMenu->addSeparator();

    menuBar()->addSeparator();


    _helpMenu = menuBar()->addMenu(tr("&Help"));
    _helpMenu->addAction(aboutAct);
    _helpMenu->addAction(aboutQtAct);
    _helpMenu->addAction(aboutPluginsAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("File");
    fileToolBar->addAction(newProjectAct);
    fileToolBar->addAction(newFileAct);
    fileToolBar->addAction(openFileAct);
    fileToolBar->addAction(saveFileAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("Edit");
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
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
    _viewMenu->addAction(dock->toggleViewAction());
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
        liteApp->emitCloseEditor(ed);
        editTabWidget->removeTab(index);
        editors.remove(w);
    }
    if (editors.empty()) {
        liteApp->emitActiveEditorChanged(NULL);
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

    liteApp->emitActiveEditorChanged(ed);
}

void MainWindow::addEditor(IEditor *editor)
{
    editors.insert(editor->widget(),editor);
    int index = editTabWidget->addTab(editor->widget(),editor->icon(),editor->fileName());
    editTabWidget->setCurrentIndex(index);
}

void MainWindow::addOutputPane(QWidget *w, const QIcon &icon, const QString &text)
{
    static int index = 0;
    index++;
    QAction *act = outputActGroup->addAction(icon,QString("%1:%2").arg(index).arg(text));
    act->setCheckable(true);
    act->setShortcut(QKeySequence(Qt::ALT+Qt::Key_0+index));
    act->setToolTip(QString("%1 Alt+%2").arg(text).arg(index));
    outputToolBar->addAction(act);
    outputStackedWidget->addWidget(w);
    outputActMap.insert(act,w);
}

void MainWindow::setCurrentOutputPane(QWidget *w)
{
    QMapIterator<QAction*,QWidget*> i(outputActMap);
    while (i.hasNext()) {
        i.next();
        if (i.value() == w) {
            lastOutputAct = i.key();
            i.key()->setChecked(true);
            if (outputStackedWidget->isHidden()) {
                outputStackedWidget->show();
            }
            outputStackedWidget->setCurrentWidget(w);
            break;
        }
    }
}

void MainWindow::addOptionPage(IOption *opt)
{
    optDialog->addPage(opt);
}

void MainWindow::createOutputWidget()
{
    outputToolBar = new QToolBar("Output Panes",this);
    outputToolBar->setObjectName("OutputPanes");
    this->addToolBar(Qt::BottomToolBarArea,outputToolBar);
    outputActGroup = new QActionGroup(this);
    connect(outputActGroup,SIGNAL(selected(QAction*)),this,SLOT(selectedOutputAct(QAction*)));
}

void MainWindow::saveFileAs()
{
    if (activeEditor) {
        QString path = liteApp->settings()->value("main/open").toString();
        QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save File As"), path, liteApp->editorNewTypeFilter());

        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                file.write(activeEditor->data());
                file.close();
                path = QFileInfo(fileName).absolutePath();
                liteApp->settings()->setValue("main/open",path);
                liteApp->loadEditor(fileName);
            }
        }
    }
}

void MainWindow::saveFile()
{
    if (activeEditor) {
        activeEditor->save();
    }
}

void MainWindow::newFile()
{
    QString path = liteApp->settings()->value("main/open").toString();

    if (activeProject) {
        path = QFileInfo(this->activeProject->filePath()).absolutePath();
    }

    QString fileName;

    fileName = QFileDialog::getSaveFileName(this,
           tr("Save New File"), path, liteApp->editorNewTypeFilter());

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
            if (QFileInfo(fileName).suffix() == "go") {
                file.write("package main");
            }
            file.close();
            path = QFileInfo(fileName).absolutePath();
            liteApp->settings()->setValue("main/open",path);
            liteApp->loadEditor(fileName);
        }
    }
}

void MainWindow::openFile()
{
    QString path = liteApp->settings()->value("main/open").toString();
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this,
           tr("Open Project or File"), path, liteApp->openTypeFilter());

    if (!fileName.isEmpty()) {
        path = QFileInfo(fileName).absolutePath();
        liteApp->settings()->setValue("main/open",path);
        if (!liteApp->loadProject(fileName)) {
            liteApp->loadEditor(fileName);
        }
    }
}

void MainWindow::about()
{
    AboutDialog * dlg = new AboutDialog(this);
    dlg->exec();
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

void MainWindow::fireTextChanged(IEditor *edit)
{
    if (activeEditor && activeEditor == edit) {
    //    astTimer->stop();
    //    astTimer->start(2000);
        liteApp->emitActiveEditorTextChanged(edit);
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
    liteApp->emitActiveProjectChanged(activeProject);
}


void MainWindow::aboutPlugins()
{
    AboutPluginsDialog dlg(this);
    dlg.resize(450,300);
    foreach (IPlugin *p, liteApp->plugins) {
        dlg.addPluginInfo(p->info());
    }

    dlg.exec();
}


void MainWindow::saveAllFile()
{
    foreach(IEditor *ed, editors) {
        ed->save();
    }
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
    QString path = liteApp->settings()->value("main/open").toString();
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

void MainWindow::closeFile()
{
    if (!activeEditor) {
        return;
    }
    QWidget *w = editors.key(activeEditor);
    if (!w) {
        return;
    }
    int index = editTabWidget->indexOf(w);
    editTabClose(index);
}

void MainWindow::closeAllFile()
{
    this->activeEditor = NULL;
    liteApp->emitActiveEditorChanged(NULL);
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
    } else {
        QMapIterator<QWidget*,IEditor*>  i(editors);
        while (i.hasNext()) {
            i.next();
            while (i.value()->filePath() == fileName) {
                ed = i.value();
                break;
            }
        }
    }
    if (!ed)
        return;
    ed->gotoLine(line,col);
}

void MainWindow::selectedOutputAct(QAction *act)
{
    QWidget *w = outputActMap.value(act,0);
    if (w == 0)
        return;
    if (lastOutputAct == act) {
        act->setChecked(false);
        lastOutputAct = NULL;
    } else {
        lastOutputAct = act;
    }
    if (lastOutputAct == NULL) {
        this->outputStackedWidget->hide();
    } else  {
        if (this->outputStackedWidget->isHidden()) {
            this->outputStackedWidget->show();
        }
        this->outputStackedWidget->setCurrentWidget(w);
    }
}

void MainWindow::options()
{
    optDialog->show();
}
