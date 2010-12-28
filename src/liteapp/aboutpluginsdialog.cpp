#include "aboutpluginsdialog.h"

#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QStandardItem>

AboutPluginsDialog::AboutPluginsDialog(QWidget *parent) :
    QDialog(parent)
{
    treeModel = new QStandardItemModel(0,2,this);
    treeModel->setHeaderData(0, Qt::Horizontal, QObject::tr("name"));
    treeModel->setHeaderData(1, Qt::Horizontal, QObject::tr("info"));

    treeView = new QTreeView(this);
    treeView->setModel(treeModel);
    treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton *closeBtn = new QPushButton(tr("close"));
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(accept()));

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(closeBtn);

    layout->addWidget(treeView);
    layout->addLayout(bottomLayout);

    setLayout(layout);
}

void AboutPluginsDialog::addPluginInfo(const QString &name, const QString &info)
{
    QList<QStandardItem*> items;
    items.append(new QStandardItem(name));
    items.append(new QStandardItem(info));
    treeModel->appendRow(items);
}
