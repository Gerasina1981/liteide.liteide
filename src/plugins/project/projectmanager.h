#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QWidget>
#include <QList>
#include "projectfile.h"
#include "../../api/iapp.h"
#include <QModelIndex>

class QTreeView;
class QStandardItemModel;
class ProjectManager : public QWidget
{
    Q_OBJECT
    enum ITEM_TYPE{
        ItemRoot = 1,
        ItemPro,
        ItemHeader,
        ItemSource,
        ItemFolder,
        ItemFile,
    };
public:
    void appendProject(ProjectFile *file);
    explicit ProjectManager(IApplication *app, QWidget *parent = 0);
    void createActions();
private slots:
    void doubleClickedTree(const QModelIndex  &index);
    void closeProject();
    void openProject();
    void newProject();
protected:
    QAction *newProjectAct;
    QAction *openProjectAct;
    QAction *closeProjectAct;
    QStandardItemModel  *model;
    QTreeView            *tree;
    QList<ProjectFile*> proFiles;
    IApplication *liteApp;
};

#endif // PROJECTMANAGER_H
