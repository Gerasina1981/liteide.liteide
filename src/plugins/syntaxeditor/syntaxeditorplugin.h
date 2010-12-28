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
    virtual void reload();
    virtual void activeEditor(QAction *undoAct, QAction *redoAct);
public slots:
    void setUndoEnabled(bool);
    void setRedoEnabled(bool);
    void  modificationChanged(bool);    
public:
    bool undoEnable;
    bool redoEnable;
    SyntaxEditor *editor;
    IEditorEvent *event;
};

class EditorFactoryImpl : public QObject, public IEditorFactory
{
    Q_OBJECT
public:
    EditorFactoryImpl(QObject *parent, IApplication *app);
    virtual QStringList fileTypes();
    virtual QString openTypes();
    virtual IEditor *create(const QString &fileName);
protected slots:
    void config();
protected:
    IApplication *liteApp;
    QAction *configAct;
    QFont    editorFont;
};


class SYNTAXEDITORSHARED_EXPORT SyntaxEditorPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    SyntaxEditorPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString info() const;
protected:
    IApplication *liteApp;
};

#endif // GOLANGEDITORPLUGIN_H
