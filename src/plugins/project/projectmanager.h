#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QWidget>
#include <QDockWidget>
#include <QList>
#include "projectfile.h"
#include "../../api/iapp.h"
#include "../../api/iproject.h"
#include <QModelIndex>

class QTreeView;
class QStandardItemModel;
class ProjectManager : public QWidget, public IProjectFactory
{
    Q_OBJECT
public:
    enum ITEM_TYPE{
        ItemRoot = 1,
        ItemFolder,
        ItemProFile,
        ItemFile,
    };
public:
    virtual QStringList projectKeys() const;
    virtual QString projectTypeFilter() const;
    virtual IProject *loadProject(const QString &filePath);

    void appendProject(ProjectFile *file);
    explicit ProjectManager(IApplication *app, QWidget *parent = 0);
    ~ProjectManager();
    void createActions();
private slots:
    void doubleClickedTree(const QModelIndex  &index);
    void closeProject();
    void openProject();
    void newProject();
protected:
    QDockWidget *parentDock;
    QAction *newProjectAct;
    QAction *openProjectAct;
    QAction *closeProjectAct;
    QStandardItemModel  *model;
    QTreeView            *tree;
    QList<ProjectFile*> proFiles;
    IApplication *liteApp;
};

#endif // PROJECTMANAGER_H
