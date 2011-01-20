#ifndef GOLANGHIGHLIGHTER_H
#define GOLANGHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QVector>
#include <QTextCharFormat>

class QTextDocument;

class GolangHighlighter: public QSyntaxHighlighter
{
    Q_OBJECT
public:
    GolangHighlighter(QTextDocument* document);
protected:
    enum
    {
        STATE_BACKQUOTES = 0x04,
        STATE_SINGLELINE_COMMENT = 0x08,
        STATE_MULTILINE_COMMENT = 0x10
    };
    virtual void highlightBlock(const QString &text);
    bool highlightPreBlock(const QString &text, int &startPos, int &endPos);
    int findQuotesEndPos(const QString &text, int startPos, const QChar &endChar);
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QRegExp         regexpQuotesAndComment;
    QTextCharFormat functionFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat identFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat quotesFormat;
};


#endif // GOLANGHIGHLIGHTER_H
