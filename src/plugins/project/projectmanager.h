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
    virtual QStringList openTypeFilter() const;
    virtual QStringList openTypeFilterList() const;
    virtual IProject *loadProject(const QString &filePath);

    void resetProjectTree();
    explicit ProjectManager(IApplication *app, QWidget *parent = 0);
    ~ProjectManager();
    void createActions();
private slots:
    void reloadProject();
    void showProjectMenu(QPoint pt);
    void doubleClickedTree(const QModelIndex  &index);
    void closeProject();
protected:
    QString reloadProjectPath;
    QModelIndex reloadProjectIndex;
    QMenu   *projectMenu;
    QDockWidget *parentDock;
    QAction *newProjectAct;
    QAction *openProjectAct;
    QAction *closeProjectAct;
    QStandardItemModel  *model;
    QTreeView            *tree;
    ProjectFile         *project;
    IApplication *liteApp;
};

#endif // PROJECTMANAGER_H
