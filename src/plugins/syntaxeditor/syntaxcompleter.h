#ifndef SYNTAXCOMPLETER_H
#define SYNTAXCOMPLETER_H

#include <QCompleter>
#include <QTextCursor>
#include <QDebug>

class SyntaxCompleter : public QCompleter
{
public:
    SyntaxCompleter(QObject *parent = 0) : QCompleter(parent)
    {
    }
    virtual bool underCursor(const QTextCursor &cur, const QString &text)
    {
        return true;
    }
    virtual void setFileName(const QString &path)
    {
        fileName = path;
    }

public:
    QString fileName;
};


#endif // SYNTAXCOMPLETER_H
