#ifndef LITEAPP_H
#define LITEAPP_H

#include "mainwindow.h"
#include "../liteapi/liteapi.h"

class LiteApp : public QObject, public IApplication
{
    Q_OBJECT

public:
    LiteApp();
    virtual void addWorkspacePane(QWidget *w, const QString &name);
    virtual void addOutputPane(QWidget *w, const QString &name);
    virtual void addEditorFactory(IEditorFactory *editFactory);
    virtual IEditorEvent *editorEvent();
    void loadPlugins(const QString &dir);
    void installPlugins();
    void uninstallPlugins();
    IEditor *openFile(const QString &fileName);
    QString openFileTypes() const;
public:
    MainWindow * mainWindow;
    QList<IPlugin*> plugins;
    QList<IEditorFactory*> editorFactorys;
private slots:
    void slotClose();
};

#endif // LITEAPP_H
