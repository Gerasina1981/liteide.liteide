#include "golangcompleter.h"

GolangCompleter::GolangCompleter(QObject *parent) : SyntaxCompleter(parent)
{
    setCaseSensitivity(Qt::CaseInsensitive);
    setWrapAround(false);
}

bool GolangCompleter::underCursor(const QTextCursor &cur, const QString &text)
{
    qDebug() << this->fileName << cur.position() << cur.positionInBlock();
    return true;
}
