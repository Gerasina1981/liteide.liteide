#include "goastviewmanager.h"
#include "goastview.h"

#include <QDockWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QVBoxLayout>

GoAstViewManager::GoAstViewManager(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent), astView(NULL)
{
    astView = new GoAstView(this);
    tree = new QTreeView;
    model = new QStandardItemModel;
    tree->setModel(model);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setHeaderHidden(true);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tree);
    layout->setMargin(0);

    setLayout(layout);
    liteApp->addAstViewFactory(this);
    parentDock = liteApp->addWorkspacePane(this,"GoAstView");
    parentDock->hide();
}

QStringList GoAstViewManager::fileTypes()
{
    return QStringList() << "go";
}

IAstView *GoAstViewManager::load(const QString &fileName)
{
    astView->update(fileName,"");
    return astView;
}
