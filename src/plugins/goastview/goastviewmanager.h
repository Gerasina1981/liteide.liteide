#ifndef GOASTVIEWMANAGER_H
#define GOASTVIEWMANAGER_H

#include <QWidget>
#include "../../api/iapp.h"
#include "../../api/iastview.h"

class QDockWidget;
class QStandardItemModel;
class QTreeView;
class GoAstView;
class GoAstViewManager : public QWidget, public IAstViewFactory
{
    Q_OBJECT
public:
    explicit GoAstViewManager(IApplication *app, QWidget *parent = 0);
    virtual QStringList fileTypes();
    virtual IAstView *load(const QString &fileName);
signals:

public slots:
protected:
    IApplication *liteApp;
    QDockWidget  *parentDock;
    QStandardItemModel  *model;
    QTreeView            *tree;
    GoAstView   *astView;
};

#endif // GOASTVIEWMANAGER_H
