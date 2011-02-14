#ifndef __LITEAPI_IEDITOR_H_
#define __LITEAPI_IEDITOR_H_

#include <QObject>
#include <QWidget>

class IEditor : public QObject
{
    Q_OBJECT
public:
    IEditor(QObject *parent) : QObject(parent)
    {
    }
    virtual QString fileName() const = 0;
    virtual QString filePath() const = 0;
    virtual QWidget *widget() const = 0;
    virtual QIcon icon() const = 0;
    virtual QByteArray data() const = 0;
    virtual void gotoLine(int line, int col) = 0;
public slots:
    virtual void save() = 0;
    virtual bool close() = 0;
    virtual void reload() = 0;
    virtual void activeEditor(QAction *undoAct, QAction *redoAct) = 0;
};

class IEditorFactory
{
public:
    virtual  ~IEditorFactory() {}
    virtual QStringList fileTypes() = 0;
    virtual QString editorTypeFilter() = 0;
    virtual IEditor *create(const QString &fileName) = 0;
};

class IEditorEvent
{
public:
    virtual ~IEditorEvent() {}
    virtual void fireDocumentChanged(IEditor *edit, bool b) = 0;
    virtual void fireDocumentSave(IEditor *edit) = 0;
    virtual void fireTextChanged(IEditor *edit) = 0;
};


#endif //__LITEAPI_IEDITOR_H_
