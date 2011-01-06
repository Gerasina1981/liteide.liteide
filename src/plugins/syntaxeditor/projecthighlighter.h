#ifndef PROJECTHIGHLIGHTER_H
#define PROJECTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>

class QTextDocument;

class ProjectHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit ProjectHighlighter(QTextDocument *document);
private:
    virtual void highlightBlock(const QString &text);
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
};

#endif // PROJECTHIGHLIGHTER_H
