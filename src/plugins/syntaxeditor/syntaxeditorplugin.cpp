#include "syntaxeditorplugin.h"
#include "golanghighlighter.h"
#include "configdialog.h"

#include <QFileInfo>

EditorImpl::EditorImpl(QObject *parent) : IEditor(parent), undoEnable(false), redoEnable(false)
{
}

void EditorImpl::setUndoEnabled(bool b)
{
    undoEnable = b;
}

void EditorImpl::setRedoEnabled(bool b)
{
    redoEnable = b;
}


QString EditorImpl::name() const
{
    return editor->currentText();
}

QString EditorImpl::fullPath() const
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
    editor->save();
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
    configAct = new QAction(tr("SyntaxEditor Config"),this);
    connect(configAct,SIGNAL(triggered()),this,SLOT(config()));
    liteApp->toolMenu()->addAction(configAct);

    editorFont.setFamily(liteApp->settings()->value("editor/family","Courier").toString());
    editorFont.setPointSize(liteApp->settings()->value("editor/fontsize",12).toInt());
}

QStringList EditorFactoryImpl::fileTypes()
{
    return QStringList() << "go";
}

QString EditorFactoryImpl::openTypes()
{
    return QObject::tr("Go Files (*.go)");
}

void EditorFactoryImpl::config()
{
    ConfigDialog dlg;
    dlg.fontFamily = editorFont.family();
    dlg.fontSize = editorFont.pointSize();
    dlg.load();
    if (dlg.exec() == QDialog::Accepted) {
        dlg.save();
        editorFont.setFamily(dlg.fontFamily);
        editorFont.setPointSize(dlg.fontSize);
        liteApp->settings()->setValue("editor/family",dlg.fontFamily);
        liteApp->settings()->setValue("editor/fontsize",dlg.fontSize);
    }
}

IEditor *EditorFactoryImpl::create(const QString &fileName)
{
    SyntaxEditor *ed = SyntaxEditor::openFile(fileName);
    if (!ed) {
        return NULL;
    }

    QString ext = QFileInfo(fileName).suffix();
    if (ext.toLower() == "go") {
        new GolangHighlighter(ed->document());
    }
    EditorImpl *impl = new EditorImpl(this);
    ed->setFont(editorFont);
    ed->setTabStopWidth(editorFont.pointSize()*4);
    impl->editor = ed;
    impl->event = liteApp->editorEvent();
    QObject::connect(ed,SIGNAL(modificationChanged(bool)),impl,SLOT(modificationChanged(bool)));
    return impl;
}

SyntaxEditorPlugin::SyntaxEditorPlugin() : liteApp(NULL)
{
}

QString SyntaxEditorPlugin::name() const
{
    return tr("SyntaxEditor");
}

QString SyntaxEditorPlugin::info() const
{
    return tr("SyntaxEditor 1.0 make by visualfc");
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

