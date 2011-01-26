#ifndef GOASTVIEWMANAGER_H
#define GOASTVIEWMANAGER_H

#include <QWidget>
#include <QModelIndex>
#include "../../api/iapp.h"
#include "../../api/iastview.h"

class QDockWidget;
class QStandardItemModel;
class QTreeView;
class GoAstView;

class GoAstViewIcon
{
public:
    GoAstViewIcon();
    QIcon iconFromTag(const QString &tag) const;
protected:
    QIcon   iconPackage;
    QIcon   iconType;
    QIcon   iconStruct;
    QIcon   iconInterface;
    QIcon   iconFunc;
    QIcon   iconFuncs;
    QIcon   iconVar;
    QIcon   iconVars;
    QIcon   iconConst;
    QIcon   iconConsts;
    QIcon   iconMethod;
};

class GoAstViewIconPriv
{
public:
    GoAstViewIconPriv();
    QIcon iconFromTag(const QString &tag) const;
protected:
    QIcon   iconPackage;
    QIcon   iconType;
    QIcon   iconStruct;
    QIcon   iconInterface;
    QIcon   iconFunc;
    QIcon   iconFuncs;
    QIcon   iconVar;
    QIcon   iconVars;
    QIcon   iconConst;
    QIcon   iconConsts;
    QIcon   iconMethod;
};

class GoAstViewManager : public QWidget, public IAstViewFactory
{
    Q_OBJECT
public:
    QStringList astFiles;
    explicit GoAstViewManager(IApplication *app, QWidget *parent = 0);
    virtual QStringList fileTypes();
    virtual IAstView *load(const QString &fileName, const QByteArray &data);
signals:

public slots:
    void doubleClickedTree(const QModelIndex &index);
    void astOutput(const QByteArray &data);
protected:
    IApplication *liteApp;
    QDockWidget  *parentDock;
    QStandardItemModel  *model;
    QTreeView            *tree;
    GoAstView   *astView;
    GoAstViewIcon   icons;
    GoAstViewIconPriv icons_p;
};

#endif // GOASTVIEWMANAGER_H
