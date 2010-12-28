#ifndef GOLANGEDITORPLUGIN_H
#define GOLANGEDITORPLUGIN_H

#include "syntaxeditor_global.h"
#include "../../liteapi/liteapi.h"

#include <QObject>
#include <QtPlugin>
#include <QMap>
#include "syntaxeditor.h"

class EditorImpl : public QObject, public IEditor
{
    Q_OBJECT
public:
    EditorImpl(QObject *parent);
    virtual ~EditorImpl() {}
    virtual QWidget *widget() const;
    virtual QString name() const;
    virtual QString fullPath() const;
    virtual QIcon icon() const;
    virtual void save();
    virtual bool close();
public slots:
    void  modificationChanged(bool);    
public:
    SyntaxEditor *editor;
    IEditorEvent *event;
};

class EditorFactoryImpl : public QObject, public IEditorFactory
{
public:
    EditorFactoryImpl(QObject *parent, IApplication *app);
    virtual QStringList fileTypes();
    virtual QString openTypes();
    virtual IEditor *create(const QString &fileName);
protected:
    IApplication *liteApp;
};


class SYNTAXEDITORSHARED_EXPORT SyntaxEditorPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    SyntaxEditorPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    IApplication *liteApp;
};

#endif // GOLANGEDITORPLUGIN_H
