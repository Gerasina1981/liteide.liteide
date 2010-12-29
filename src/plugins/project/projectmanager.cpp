#include "projectmanager.h"
#include "projectwizard.h"


#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QVariant>

ProjectManager::ProjectManager(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent)
{
    tree = new QTreeView;
    model = new QStandardItemModel;
    tree->setModel(model);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setHeaderHidden(true);
    connect(tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tree);
    layout->setMargin(0);

    setLayout(layout);

    createActions();

    liteApp->addWorkspacePane(this,"Projects");
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
    if (file->openProject(fileName)) {
        appendProject(file);
    } else {
        delete file;
    }
}

void ProjectManager::closeProject()
{
}

void ProjectManager::appendProject(ProjectFile *file)
{
    proFiles.append(file);
    QStandardItem *item = new QStandardItem(file->projectName());
    model->appendRow(item);
    item->setData(ItemRoot);
    item->setData(file->filePath(),Qt::UserRole+2);

    QStandardItem *pro = new QStandardItem(file->fileName());
    pro->setData(ItemProFile);
    item->appendRow(pro);

    QMap<QString,QString> fileMap;
    fileMap.insert("GOFILES",tr("Gofiles"));
    fileMap.insert("HEADERS",tr("Headers"));
    fileMap.insert("SOURCES",tr("Sources"));

    QMapIterator<QString,QString> i(fileMap);
    while(i.hasNext()) {
        i.next();
        QStringList files = file->values(i.key());
        if (!files.isEmpty()) {
            QStandardItem *folder = new QStandardItem(i.value());
            folder->setData(ItemFolder);
            item->appendRow(folder);
            foreach(QString v, files) {
                QStandardItem *fileItem = new QStandardItem(v);
                fileItem->setData(ItemFile);
                folder->appendRow(fileItem);
            }
        }
    }

    tree->expand(model->indexFromItem(item));    
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
            liteApp->openFile(filePath);
            return;
        }
    }
}
