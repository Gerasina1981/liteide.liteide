#ifndef GOLANGCOMPLETER_H
#define GOLANGCOMPLETER_H

#include "syntaxcompleter.h"

class GolangCompleter : public SyntaxCompleter
{
public:
    virtual bool underCursor(const QTextCursor &cur, const QString &text);
    GolangCompleter(QObject *parent);
};

#endif // GOLANGCOMPLETER_H
