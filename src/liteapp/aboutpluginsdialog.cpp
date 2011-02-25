#include "aboutpluginsdialog.h"

#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>

AboutPluginsDialog::AboutPluginsDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("About Plugins");

    treeModel = new QStandardItemModel(0,4,this);
    treeModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
    treeModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Anchor"));
    treeModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Info"));
    treeModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Version"));


    treeView = new QTreeView(this);
    treeView->setModel(treeModel);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView->setItemsExpandable(true);
    treeView->setRootIsDecorated(false);
    treeView->header()->setResizeMode(QHeaderView::ResizeToContents);

    QPushButton *closeBtn = new QPushButton(tr("Close"));
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(closeBtn);

    layout->addWidget(treeView);
    layout->addLayout(bottomLayout);

    setLayout(layout);
}

void AboutPluginsDialog::addPluginInfo(const PluginInfo &info)
{
    QList<QStandardItem*> items;
    items.append(new QStandardItem(info.name));
    items.append(new QStandardItem(info.anchor));
    items.append(new QStandardItem(info.info));
    items.append(new QStandardItem(info.ver));
    treeModel->appendRow(items);
}
