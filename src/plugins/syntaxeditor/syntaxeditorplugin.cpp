#include "syntaxeditorplugin.h"
#include "golanghighlighter.h"

#include <QFileInfo>

EditorImpl::EditorImpl(QObject *parent) : QObject(parent)
{

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

void EditorImpl::modificationChanged(bool b)
{
    event->fireDocumentChanged(this,b);
}

EditorFactoryImpl::EditorFactoryImpl(QObject *parent,IApplication *app)
    : QObject(parent), liteApp(app)
{

}

QStringList EditorFactoryImpl::fileTypes()
{
    return QStringList() << "go";
}

QString EditorFactoryImpl::openTypes()
{
    return QObject::tr("Go Files (*.go)");
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
    impl->editor = ed;
    impl->event = liteApp->editorEvent();
    QObject::connect(ed,SIGNAL(modificationChanged(bool)),impl,SLOT(modificationChanged(bool)));
    return impl;
}

SyntaxEditorPlugin::SyntaxEditorPlugin() : liteApp(NULL)
{
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

