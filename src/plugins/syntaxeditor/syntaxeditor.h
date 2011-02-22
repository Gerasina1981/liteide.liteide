#ifndef GOLANGEDITOR_H
#define GOLANGEDITOR_H

#include <QPlainTextEdit>
#include <QTextEdit>
#include "syntaxcompleter.h"
#include "../../api/iapp.h"

class SyntaxEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    void indentText(QTextDocument *doc, QTextCursor cur, bool bIndent);
    void indentBlock(QTextBlock block, bool bIndent);
    void indentCursor(QTextCursor cur, bool bIndent);
    void gotoLine(int line, int column);
    QString textUnderCursor() const;
    void setCompleter(SyntaxCompleter *c);
    SyntaxCompleter *completer() const;
    bool autoIndent;
    bool autoBlock;
    void areaPaintEvent(QPaintEvent *event);
    SyntaxEditor(IApplication *app, QWidget *parent = 0);
    void newFile();
    void reload();
    QString currentText() { return curText; }
    bool loadFile(const QString &fileName);
    QByteArray data() const;
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }
signals:
    void update();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusInEvent(QFocusEvent *e);
    virtual bool event(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *e);
public:
    int editorAreaWidth();
public slots:
    void loadConfig();
protected slots:
    void highlightCurrentLine();
    void insertCompletion(const QString& completion);
    void updateAreaWidth(int newBlockCount);
    void updateEditorArea(const QRect &, int);
public:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    QString curText;
    QFont   curFont;

    bool isUntitled;
    QWidget *editorArea;
    SyntaxCompleter *editCompleter;
    IApplication *liteApp;
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
