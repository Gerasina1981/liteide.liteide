#ifndef __LITEAPI_IAPP_H__
#define __LITEAPI_IAPP_H__

#include <QWidget>
#include <QSettings>
#include <QMenu>

class IEditorFactory;
class IEditorEvent;
class IEditor;

class IApplication
{
public:
    virtual ~IApplication() {}
    virtual void addWorkspacePane(QWidget *w, const QString &name) = 0;
    virtual void addOutputPane(QWidget *w, const QString &name) = 0;
    virtual void addEditorFactory(IEditorFactory *editFactory) = 0;
    virtual IEditorEvent *editorEvent() = 0;
    virtual QSettings *settings() = 0;
    virtual QMenu *fileMenu() = 0;
    virtual QMenu *viewMenu() = 0;
    virtual QMenu *editMenu() = 0;
    virtual QMenu *toolMenu() = 0;
    virtual IEditor *activeEditor() = 0;
};

#endif //__LITEAPI_IAPP_H__
