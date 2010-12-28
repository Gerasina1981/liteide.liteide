#ifndef LITEAPI_H
#define LITEAPI_H

#include <QObject>
#include <QIcon>

class IContext
{
public:
    virtual ~IContext() {}
    virtual QWidget *widget() const = 0;
};

class IEditor : public IContext
{
public:
    virtual QString name() const = 0;
    virtual QString fullPath() const = 0;
    virtual QIcon icon() const = 0;
    virtual void save() = 0;
    virtual bool close() = 0;
};

class IEditorFactory
{
public:
    virtual  ~IEditorFactory() {}
    virtual QStringList fileTypes() = 0;
    virtual QString openTypes() = 0;
    virtual IEditor *create(const QString &fileName) = 0;
};

class IEditorEvent
{
public:
    virtual ~IEditorEvent() {}
    virtual void fireDocumentChanged(IEditor *edit, bool b) = 0;
};

class IApplication
{
public:
    virtual ~IApplication() {}
    virtual void addWorkspacePane(QWidget *w, const QString &name) = 0;
    virtual void addOutputPane(QWidget *w, const QString &name) = 0;
    virtual void addEditorFactory(IEditorFactory *editFactory) = 0;
    virtual IEditorEvent *editorEvent() = 0;
};

class IPlugin
{
public:
    virtual ~IPlugin() {}
    virtual void install(IApplication *app) = 0;
    virtual void uninstall() = 0;
};

Q_DECLARE_INTERFACE(IPlugin,"lite.IPlugin/1.0")


#endif // LITEAPI_H
