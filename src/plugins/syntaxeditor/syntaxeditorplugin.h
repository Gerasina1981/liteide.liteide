#ifndef GOLANGEDITORPLUGIN_H
#define GOLANGEDITORPLUGIN_H

#include "syntaxeditor_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/ieditor.h"

#include <QObject>
#include <QtPlugin>
#include <QMap>
#include "syntaxeditor.h"

class SyntaxEditOption;
class EditorImpl : public IEditor
{
    Q_OBJECT
public:
    EditorImpl(IApplication *app,QObject *parent = 0);
    virtual QWidget *widget() const;
    virtual QString fileName() const;
    virtual QString filePath() const;
    virtual QIcon icon() const;
    virtual void save();
    virtual bool close();
    virtual void reload();
    virtual void activeEditor(QAction *undoAct, QAction *redoAct);
    virtual QByteArray data() const;
    virtual void gotoLine(int line, int col);
public slots:
    void textChanged();
    void setUndoEnabled(bool);
    void setRedoEnabled(bool);
    void  modificationChanged(bool);    
public:
    bool undoEnable;
    bool redoEnable;
    SyntaxEditor *editor;
    IEditorEvent *event;
    IApplication *liteApp;
};

class EditorFactoryImpl : public QObject, public IEditorFactory
{
    Q_OBJECT
public:
    EditorFactoryImpl(QObject *parent, IApplication *app);
    virtual QStringList fileTypes();
    virtual QString editorTypeFilter();
    virtual IEditor *create(const QString &fileName);
protected:
    IApplication *liteApp;
    QList<SyntaxEditor*>    editors;
    QAction *configAct;
    SyntaxEditOption *opt;
};


class /*SYNTAXEDITORSHARED_EXPORT*/ SyntaxEditorPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    SyntaxEditorPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString anchor() const;
    virtual QString info() const;
protected:
    IApplication *liteApp;
};

#endif // GOLANGEDITORPLUGIN_H
