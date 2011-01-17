#ifndef GOLANGEDITOR_H
#define GOLANGEDITOR_H

#include <QPlainTextEdit>
#include <QTextEdit>
#include "syntaxcompleter.h"

class SyntaxEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    QString textUnderCursor() const;
    void setCompleter(SyntaxCompleter *c);
    SyntaxCompleter *completer() const;
    bool autoIndent;
    bool autoBlock;
    void areaPaintEvent(QPaintEvent *event);
    SyntaxEditor();
    void newFile();
    void reload();
    QString currentText() { return curText; }
    static SyntaxEditor *openFile(const QString &fileName);
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual bool event(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *e);
public:
    int editorAreaWidth();
protected slots:
    void insertCompletion(const QString& completion);
    void updateAreaWidth(int newBlockCount);
    void updateEditorArea(const QRect &, int);
protected:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    QString curText;

    bool isUntitled;
    QWidget *editorArea;
    SyntaxCompleter *editCompleter;
};

class SyntaxEditorArea : public QWidget
{
public:
    SyntaxEditorArea(SyntaxEditor *ed) : QWidget(ed) {
        editor = ed;
    }

    QSize sizeHint() const {
        return QSize(editor->editorAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {        
        editor->areaPaintEvent(event);
    }

private:
    SyntaxEditor *editor;
};


#endif // GOLANGEDITOR_H
