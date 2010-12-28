#ifndef ABOUTPLUGINSDIALOG_H
#define ABOUTPLUGINSDIALOG_H

#include <QDialog>

class QTreeView;
class QStandardItemModel;
class AboutPluginsDialog : public QDialog
{
    Q_OBJECT
public:
    void addPluginInfo(const QString &name, const QString &info);
    explicit AboutPluginsDialog(QWidget *parent = 0);

signals:

public slots:

private:
    QTreeView  *treeView;
    QStandardItemModel *treeModel;
};

#endif // ABOUTPLUGINSDIALOG_H
