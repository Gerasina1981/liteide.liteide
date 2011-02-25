#include "syntaxeditorplugin.h"
#include "golanghighlighter.h"
#include "golangcompleter.h"
#include "projecthighlighter.h"
#include "configdialog.h"

#include <QFileInfo>
#include <QDebug>

EditorImpl::EditorImpl(IApplication *app, QObject *parent)
    : liteApp(app), IEditor(parent), undoEnable(false), redoEnable(false)
{
}

void EditorImpl::textChanged()
{
    event->fireTextChanged(this);
}

QByteArray EditorImpl::data() const
{
    return editor->data();
}

void EditorImpl::gotoLine(int line, int col)
{
    editor->setFocus();
    editor->gotoLine(line,col);
}

void EditorImpl::setUndoEnabled(bool b)
{
    undoEnable = b;
}

void EditorImpl::setRedoEnabled(bool b)
{
    redoEnable = b;
}


QString EditorImpl::fileName() const
{
    return editor->currentText();
}

QString EditorImpl::filePath() const
{
    return editor->currentFile();
}

QWidget *EditorImpl::widget() const
{
    return editor;
}
QIcon EditorImpl::icon() const
{
    return QIcon();
}

void EditorImpl::save()
{
    if (editor->save()) {
        liteApp->editorEvent()->fireDocumentSave(this);
    }
}

bool EditorImpl::close()
{
    return editor->close();
}

void EditorImpl::reload()
{
    editor->reload();
}

void EditorImpl::activeEditor(QAction *undoAct, QAction *redoAct)
{
    undoAct->setEnabled(undoEnable);
    redoAct->setEnabled(redoEnable);
    connect(editor,SIGNAL(undoAvailable(bool)),undoAct,SLOT(setEnabled(bool)));
    connect(editor,SIGNAL(redoAvailable(bool)),redoAct,SLOT(setEnabled(bool)));
    connect(editor,SIGNAL(undoAvailable(bool)),this,SLOT(setUndoEnabled(bool)));
    connect(editor,SIGNAL(redoAvailable(bool)),this,SLOT(setRedoEnabled(bool)));

    connect(undoAct,SIGNAL(triggered()),editor,SLOT(undo()));
    connect(redoAct,SIGNAL(triggered()),editor,SLOT(redo()));
}


void EditorImpl::modificationChanged(bool b)
{
    event->fireDocumentChanged(this,b);
}

EditorFactoryImpl::EditorFactoryImpl(QObject *parent,IApplication *app)
    : QObject(parent), liteApp(app)
{
    opt = new SyntaxEditOption(liteApp);
    liteApp->mainWindow()->addOptionPage(opt);
}

QStringList EditorFactoryImpl::fileTypes()
{
    return QStringList() << "go" << "cgo";
}

QStringList EditorFactoryImpl::openTypeFilter()
{
    return QStringList() << "Go Files (*.go *.goc)";
}

QStringList EditorFactoryImpl::newTypeFilter()
{
    return QStringList() << "Go Files (*.go)" << "All Files (*)";
}


QStringList EditorFactoryImpl::openTypeFilterList()
{
    return QStringList() << "*.go" << "*.goc";
}

IEditor *EditorFactoryImpl::create(const QString &fileName)
{
    SyntaxEditor *ed = new SyntaxEditor(liteApp);
    QString ext = QFileInfo(fileName).suffix();
    if (!ed->loadFile(fileName)) {
        delete ed;
        return NULL;
    }
    if (ext.toLower() == "go") {
        GolangHighlighter *l = new GolangHighlighter(ed->document());
        GolangCompleter * c = new GolangCompleter(ed);
        c->allWords = l->allWords;
        ed->setCompleter(c);
        connect(ed,SIGNAL(update()),c,SLOT(updateEditor()));
    } else if (ext.toLower() == "pro") {
        new ProjectHighlighter(ed->document());
    }
    EditorImpl *impl = new EditorImpl(liteApp,this);

    connect(opt,SIGNAL(reloadConfig()),ed,SLOT(loadConfig()));

    impl->editor = ed;
    impl->event = liteApp->editorEvent();
    ed->loadConfig();

    editors.append(ed);
    connect(ed,SIGNAL(textChanged()),impl,SLOT(textChanged()));
    QObject::connect(ed,SIGNAL(modificationChanged(bool)),impl,SLOT(modificationChanged(bool)));
    return impl;
}

SyntaxEditorPlugin::SyntaxEditorPlugin() : liteApp(NULL)
{
}

const PluginInfo & SyntaxEditorPlugin::info() const
{
    static PluginInfo s;
    s.name = "syntaxeditor";
    s.anchor = "visualfc";
    s.info = tr("Syntax Editor");
    s.ver = "0.2.1";
    return s;
}

void SyntaxEditorPlugin::install(IApplication *app)
{
    liteApp = app;
    liteApp->addEditorFactory(new EditorFactoryImpl(this,app)); 
}

void SyntaxEditorPlugin::uninstall()
{

}

Q_EXPORT_PLUGIN(SyntaxEditorPlugin)
