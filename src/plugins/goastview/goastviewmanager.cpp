#include "goastviewmanager.h"
#include "goastview.h"

#include <QDockWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QDebug>
/*
const (
        tag_package = "p"
        tag_type = "t"
        tag_struct = "s"
        tag_interface = "i"
        tag_value = "v"
        tag_const = "c"
        tag_value_folder = "+v"
        tag_const_folder = "+c"
        tag_func = "f"
        tag_func_folder = "+f"
        tag_type_method = "m"
        tag_type_factor = "a"
)
*/
GoAstViewIcon::GoAstViewIcon() :
    iconPackage(QIcon(":/images/package.png")),
    iconType(QIcon(":/images/type.png")),
    iconStruct(QIcon(":/images/struct.png")),
    iconInterface(QIcon(":/images/interface.png")),
    iconFunc(QIcon(":/images/func.png")),
    iconFuncs(QIcon(":/images/funcs.png")),
    iconVar(QIcon(":/images/var.png")),
    iconVars(QIcon(":/images/vars.png")),
    iconConst(QIcon(":/images/const.png")),
    iconConsts(QIcon(":/images/consts.png")),
    iconMethod(QIcon(":/images/method.png"))
{

}
QIcon GoAstViewIcon::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "+v")
        return iconVars;
    else if (tag == "+c")
        return iconConsts;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "+f")
        return iconFuncs;
    else if (tag == "m")
        return iconMethod;
    else if (tag == "a")
        return iconFunc;
    return QIcon();
}

GoAstViewIconPriv::GoAstViewIconPriv() :
    iconPackage(QIcon(":/images/package.png")),
    iconType(QIcon(":/images/type_p.png")),
    iconStruct(QIcon(":/images/struct_p.png")),
    iconInterface(QIcon(":/images/interface_p.png")),
    iconFunc(QIcon(":/images/func_p.png")),
    iconFuncs(QIcon(":/images/funcs.png")),
    iconVar(QIcon(":/images/var_p.png")),
    iconVars(QIcon(":/images/vars.png")),
    iconConst(QIcon(":/images/const_p.png")),
    iconConsts(QIcon(":/images/consts.png")),
    iconMethod(QIcon(":/images/method_p.png"))
{

}
QIcon GoAstViewIconPriv::iconFromTag(const QString &tag) const
{
    if (tag == "p")
        return iconPackage;
    else if (tag == "t")
        return iconType;
    else if (tag == "i")
        return iconInterface;
    else if (tag == "s")
        return iconStruct;
    else if (tag == "v")
        return iconVar;
    else if (tag == "c")
        return iconConst;
    else if (tag == "+v")
        return iconVars;
    else if (tag == "+c")
        return iconConsts;
    else if (tag == "f")
        return iconFunc;
    else if (tag == "+f")
        return iconFuncs;
    else if (tag == "m")
        return iconMethod;
    else if (tag == "a")
        return iconFunc;
    return QIcon();
}

GoAstViewManager::GoAstViewManager(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent), astView(NULL)
{
    astView = new GoAstView(app,this);
    connect(astView,SIGNAL(astOutput(QByteArray)),this,SLOT(astOutput(QByteArray)));
    tree = new QTreeView;
    model = new QStandardItemModel;
    tree->setModel(model);
    tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tree->setHeaderHidden(true);
    tree->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tree,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(doubleClickedTree(QModelIndex)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tree);
    layout->setMargin(0);

    setLayout(layout);
    liteApp->addAstViewFactory(this);
    parentDock = liteApp->addWorkspacePane(this,"AstView");
    parentDock->hide();
}

QStringList GoAstViewManager::fileTypes()
{
    return QStringList() << "go";
}

IAstView *GoAstViewManager::load(const QString &fileName)
{
    static bool b = true;
    if (b) {
        b = false;
        parentDock->show();
    }
    astView->update(fileName,"");
    return astView;
}
/*
const (
        tag_package = "p"
        tag_type = "t"
        tag_struct = "s"
        tag_interface = "i"
        tag_value = "v"
        tag_const = "c"
        tag_value_folder = "+v"
        tag_const_folder = "+c"
        tag_func = "f"
        tag_func_folder = "+f"
        tag_type_method = "m"
        tag_type_factor = "a"
)
*/

// level:tag:name:source:x:y
void GoAstViewManager::astOutput(const QByteArray &data)
{
    model->clear();

    QList<QByteArray> array = data.split('\n');
    QMap<int,QStandardItem*> items;
    foreach (QByteArray line, array) {
        QList<QByteArray> info = line.split(':');
        if (info.size() < 3) {
            continue;
        }
        int level = info[0].toInt();
        QString tag = info[1];
        QString name = info[2];
        if (name.isEmpty() || tag.isEmpty()) {
            continue;
        }
        QStandardItem *item = new QStandardItem(name);
        item->setData(line);
        if (name.at(0).isLower()) {
            item->setIcon(icons_p.iconFromTag(tag));
        } else {
            item->setIcon(icons.iconFromTag(tag));
        }
        QStandardItem *parent = items.value(level-1,0);
        if (parent ) {
            parent->appendRow(item);
        } else {
            model->appendRow(item);
        }
        items[level] = item;
        if (tag == "s" || tag == "p") {
            tree->expand(model->indexFromItem(item));
        }
    }
}

void GoAstViewManager::doubleClickedTree(const QModelIndex &index)
{
    QStandardItem *item = model->itemFromIndex(index);
    if (item == NULL)
        return;
    QByteArray line = item->data().toByteArray();

    QList<QByteArray> infos = line.split(':');
    // level:tag:name:source:x:y
    if (infos.size() >= 6) {
        QString fileName = infos[3];
        int x = infos[4].toInt();
        int y = infos[5].toInt();
        liteApp->gotoLine(fileName,x,y);
    }
}
