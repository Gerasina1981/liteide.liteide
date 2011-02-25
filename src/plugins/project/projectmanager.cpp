#include "projectmanager.h"

#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QVariant>
#include <QDebug>

ProjectManager::ProjectManager(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent), project(NULL)
{
    tree = new QTreeView;
    model = new QStandardItemModel;
    tree->setModel(model);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setHeaderHidden(true);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));
    connect(tree,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showProjectMenu(QPoint)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tree);
    layout->setMargin(0);

    setLayout(layout);

    liteApp->addProjectFactory(this);
    parentDock = liteApp->mainWindow()->addWorkspacePane(this,"Projects");
}

ProjectManager::~ProjectManager()
{
//    qDebug() << "~";
}

void ProjectManager::createActions()
{
    newProjectAct = new QAction(tr("New Project"),this);
    newProjectAct->setToolTip(tr("Create New Project"));
    connect(newProjectAct,SIGNAL(triggered()),this,SLOT(newProject()));

    openProjectAct = new QAction(tr("Open Project"),this);
    openProjectAct->setToolTip(tr("Open Project"));
    connect(openProjectAct,SIGNAL(triggered()),this,SLOT(openProject()));

    closeProjectAct = new QAction(tr("Close Project"),this);
    closeProjectAct->setToolTip(tr("Close Project"));
    connect(closeProjectAct,SIGNAL(triggered()),this,SLOT(closeProject()));

    QMenu *fileMenu = liteApp->mainWindow()->fileMenu();
    QAction *seperator = fileMenu->insertSeparator(fileMenu->actions()[0]);
    fileMenu->insertActions(seperator,QList<QAction*>() << newProjectAct
                            << openProjectAct << closeProjectAct);
}

QStringList ProjectManager::openTypeFilter() const
{
    return QStringList() << "Project or Makefile (*.pro Makefile)";
}

QStringList ProjectManager::openTypeFilterList() const
{
    return QStringList() << "*.pro" << "Makefile";
}

QStringList ProjectManager::projectKeys() const
{
    return QStringList() << "pro";
}

IProject *ProjectManager::loadProject(const QString &filePath)
{
    if (project && project->filePath() == filePath) {
        this->reloadProject();
        return project;
    }

    ProjectFile * file = new ProjectFile(this);
    if (file->open(filePath)) {
        closeProject();
        project = file;
        connect(project,SIGNAL(closeProject()),this,SLOT(closeProject()));
        connect(project,SIGNAL(reloadProject()),this,SLOT(reloadProject()));
        resetProjectTree();
        liteApp->projectEvent()->fireProjectChanged(project);
        return project;
    } else {
        delete file;
    }
    return NULL;
}


void ProjectManager::resetProjectTree()
{
    if (parentDock->isHidden()) {
        parentDock->show();
    }

    model->clear();

    QStandardItem *root = new QStandardItem(project->displayName());
    model->appendRow(root);
    root->setData(ItemRoot);
    root->setData(project->filePath(),Qt::UserRole+2);

    QStandardItem *item = new QStandardItem(project->fileName());
    item->setData(ItemProFile);
    root->appendRow(item);

    QMap<QString,QString> fileMap;
    fileMap.insert("GOFILES",tr("GOFILES"));
    fileMap.insert("CGOFILES",tr("CGOFILES"));
    fileMap.insert("HEADERS",tr("Headers"));
    fileMap.insert("SOURCES",tr("Sources"));

    QMapIterator<QString,QString> i(fileMap);
    while(i.hasNext()) {
        i.next();
        QStringList files = project->values(i.key());
        if (!files.isEmpty()) {
            QStandardItem *folder = new QStandardItem(i.value());
            folder->setData(ItemFolder);
            root->appendRow(folder);
            foreach(QString file, files) {
                QStandardItem *fileItem = new QStandardItem(file);
                fileItem->setData(ItemFile);
                folder->appendRow(fileItem);
            }
        }
    }   

    QStringList paks = project->values("PACKAGE");
    foreach (QString pak,paks) {
        QStringList files = project->values(pak);
        if (!files.isEmpty()) {
            QStandardItem *folder = new QStandardItem("package "+pak);
            folder->setData(ItemFolder);
            root->appendRow(folder);
            foreach(QString file, files) {
                QStandardItem *fileItem = new QStandardItem(file);
                fileItem->setData(ItemFile);
                folder->appendRow(fileItem);
            }
        }
    }

    tree->expandAll();//(model->indexFromItem(root));
}

void ProjectManager::doubleClickedTree(const QModelIndex  &index)
{
    if(!index.isValid())
        return;
    QStandardItem *item = model->itemFromIndex(index);
    bool ok;
    int type = item->data().toInt(&ok);
    if (!ok)
        return;

    if (type != ItemProFile && type != ItemFile)
        return;

    QString fileName = item->text();

    QModelIndex root = index.parent();
    bool find = false;
    while (root.isValid()) {
        QStandardItem *item = model->itemFromIndex(root);
        bool ok;
        int type = item->data().toInt(&ok);
        if (ok && type == ItemRoot) {
            find = true;
            break;
        }
        root = root.parent();
    }
    if (!find)
        return;

    QString filePath = QFileInfo(QDir(project->projectPath()),fileName).absoluteFilePath();
    liteApp->loadEditor(filePath);
}

void ProjectManager::showProjectMenu(QPoint pos)
{
    return;
    QMenu * menu = new QMenu(this);
    QModelIndex index = tree->indexAt(pos);
    if (!index.isValid())
        return;
    QStandardItem *item = model->itemFromIndex(index);
    bool ok;
    if (item->data().toInt(&ok) != ItemRoot)
        return;
    reloadProjectIndex = index;
    reloadProjectPath = item->data(Qt::UserRole+2).toString();

    menu->popup(tree->mapToGlobal(pos));
}

void ProjectManager::reloadProject()
{
    if (project) {
        resetProjectTree();
    }
}

void ProjectManager::closeProject()
{
    liteApp->projectEvent()->fireProjectClose(project);
    model->clear();
    if (project) {
        project->deleteLater();
    }
    project = NULL;
}
