#ifndef GOLANGCOMPLETER_H
#define GOLANGCOMPLETER_H

#include "syntaxcompleter.h"

class QStringListModel;
class GolangCompleter : public SyntaxCompleter
{
    Q_OBJECT
public:
    virtual bool underCursor(const QTextCursor &cur, const QString &text);
    GolangCompleter(QObject *parent);
    QStringListModel *model;
    QSharedPointer< QSet<QString> >   allWords;
public slots:
    void updateEditor();
};

#endif // GOLANGCOMPLETER_H
