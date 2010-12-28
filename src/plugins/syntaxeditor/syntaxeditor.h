#ifndef GOLANGEDITOR_H
#define GOLANGEDITOR_H

#include <QPlainTextEdit>

class SyntaxEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    SyntaxEditor();
    void newFile();
    QString currentText() { return curText; }
    static SyntaxEditor *openFile(const QString &fileName);
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString currentFile() { return curFile; }

 protected:
     void closeEvent(QCloseEvent *event);

 private slots:
     void documentWasModified();

 private:
     bool maybeSave();
     void setCurrentFile(const QString &fileName);
     QString strippedName(const QString &fullFileName);

     QString curFile;
     QString curText;

     bool isUntitled;
};

#endif // GOLANGEDITOR_H
