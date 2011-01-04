#include "projectmanager.h"
#include "projectwizard.h"


#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QVariant>
#include <QDebug>

ProjectManager::ProjectManager(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent)
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

    reloadProjectAct = new QAction(tr("Reload Project"),this);
    connect(reloadProjectAct,SIGNAL(triggered()),this,SLOT(reloadProject()));

    liteApp->addProjectFactory(this);
    parentDock = liteApp->addWorkspacePane(this,"Projects");
    parentDock->hide();
}

ProjectManager::~ProjectManager()
{
//    qDebug() << "~";
    qDeleteAll(proFiles);
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

    QMenu *fileMenu = liteApp->fileMenu();
    QAction *seperator = fileMenu->insertSeparator(fileMenu->actions()[0]);
    fileMenu->insertActions(seperator,QList<QAction*>() << newProjectAct
                            << openProjectAct << closeProjectAct);
}

void ProjectManager::newProject()
{
    ProjectWizard wiz;
    if (wiz.exec() == QDialog::Accepted) {
        //IProject * proj = createProject(wiz.projectLocation,wiz.projectName);
    }
}

void ProjectManager::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(NULL,tr("Open File"),
                                                    "",
                                                    tr("project (*.pro)"));
    if (fileName.isEmpty())
        return;

    ProjectFile * file = new ProjectFile(this);
    if (file->open(fileName)) {
        appendProject(file);
    } else {
        delete file;
    }
}

void ProjectManager::closeProject()
{
}

QString ProjectManager::projectTypeFilter() const
{
    return tr("Project files (*.pro)");
}

QStringList ProjectManager::projectKeys() const
{
    return QStringList() << "pro";
}

IProject *ProjectManager::loadProject(const QString &filePath)
{
    foreach(ProjectFile *file, proFiles ) {
        if (file->filePath() == filePath) {
            liteApp->projectEvent()->fireProjectChanged(file);
            return file;
        }
    }

    QString ext = QFileInfo(filePath).suffix();
    if (ext.toLower() == "pro") {
        ProjectFile * file = new ProjectFile;//(this);
        if (file->open(filePath)) {
            appendProject(file);
            liteApp->projectEvent()->fireProjectChanged(file);
            return file;
        } else {
            delete file;
        }
    }
    return NULL;
}


void ProjectManager::appendProject(ProjectFile *pro)
{
    if (parentDock->isHidden()) {
        parentDock->show();
    }

    proFiles.append(pro);
    QStandardItem *root = new QStandardItem(pro->displayName());
    model->appendRow(root);
    root->setData(ItemRoot);
    root->setData(pro->filePath(),Qt::UserRole+2);

    QStandardItem *item = new QStandardItem(pro->fileName());
    item->setData(ItemProFile);
    root->appendRow(item);

    QMap<QString,QString> fileMap;
    fileMap.insert("GOFILES",tr("Gofiles"));
    fileMap.insert("HEADERS",tr("Headers"));
    fileMap.insert("SOURCES",tr("Sources"));

    QMapIterator<QString,QString> i(fileMap);
    while(i.hasNext()) {
        i.next();
        QStringList files = pro->values(i.key());
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

    QStringList paks = pro->values("PACKAGE");
    foreach (QString pak,paks) {
        QStringList files = pro->values(pak);
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

    tree->expand(model->indexFromItem(root));
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
    QString proFilePath = root.data(Qt::UserRole+2).toString();

    foreach(ProjectFile *file, proFiles) {
        if (file->filePath() == proFilePath) {
            QString filePath = QFileInfo(QDir(file->projectPath()),fileName).absoluteFilePath();
            liteApp->loadEditor(filePath);
            return;
        }
    }
}

void ProjectManager::showProjectMenu(QPoint pos)
{
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

    menu->addAction(reloadProjectAct);
    menu->popup(tree->mapToGlobal(pos));
}

void ProjectManager::reloadProject()
{
   if (reloadProjectPath.isEmpty())
       return;

   foreach(ProjectFile *file, proFiles) {
       if (file->filePath() == reloadProjectPath) {
         //  QString filePath = QFileInfo(QDir(file->projectPath()),fileName).absoluteFilePath();
         //  liteApp->loadEditor(filePath);
           model->removeRow(reloadProjectIndex.row());
           proFiles.removeOne(file);
           delete file;
           this->loadProject(reloadProjectPath);
           break;
       }
   }

   reloadProjectPath.clear();
}
