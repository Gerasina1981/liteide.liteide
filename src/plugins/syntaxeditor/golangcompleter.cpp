#include "golangcompleter.h"

#include <QTextBlock>
#include <QStringListModel>

GolangCompleter::GolangCompleter(QObject *parent) : SyntaxCompleter(parent)
{
    model = new QStringListModel;
    setModel(model);
    setCaseSensitivity(Qt::CaseSensitive);
    setWrapAround(false);
    setCompletionMode(PopupCompletion);
}

bool GolangCompleter::underCursor(const QTextCursor &cur, const QString &text)
{
    //qDebug() << this->fileName << cur.position() << cur.block().blockNumber();
    return true;
}

void GolangCompleter::updateEditor()
{
    model->setStringList(this->allWords->toList());
}
