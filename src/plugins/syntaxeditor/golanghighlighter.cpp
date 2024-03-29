#include "golanghighlighter.h"
#include <QDebug>

GolangHighlighter::GolangHighlighter(QTextDocument* document):
    QSyntaxHighlighter(document), allWords(new QSet<QString>)
{
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    identFormat.setForeground(Qt::darkBlue);

    functionFormat.setForeground(Qt::blue);

    numberFormat.setForeground(Qt::darkMagenta);

    quotesFormat.setForeground(Qt::darkGreen);

    singleLineCommentFormat.setForeground(Qt::darkCyan);
    multiLineCommentFormat.setForeground(Qt::darkCyan);

    QString word;
    HighlightingRule rule;
    //number
    rule.pattern = QRegExp("(\\b|\\.)([0-9]+|0[xX][0-9a-fA-F]+|0[0-7]+)(\\.[0-9]+)?([eE][+-]?[0-9]+i?)?\\b");
    rule.format = numberFormat;

    highlightingRules.push_back(rule);

    //function
    rule.pattern = QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_]+\\s*(?=\\()");
    rule.format = functionFormat;

    highlightingRules.push_back(rule);

    //indent
    rule.pattern = QRegExp("\\b"
                           "(bool|byte|complex64|complex128|float32|float64|"
                           "int8|int16|int32|int64|string|uint8|uint16|uint32|uint64|"
                           "int|uint|uintptr|"
                           "true|false|iota|"
                           "nil|"
                           "append|cap|close|closed|complex|copy|imag|len|"
                           "make|new|panic|print|println|real|recover)"
                           "\\b");
    rule.format = identFormat;
    highlightingRules.append(rule);
    //
    word = rule.pattern.pattern();
    word.remove("\\b");
    word.remove("(");
    word.remove(")");
    foreach(QString v, word.split("|")) {
        allWords->insert(v);
    }

    //keyword
    rule.pattern = QRegExp("\\b"
                           "(break|default|func|interface|select|"
                           "case|defer|go|map|struct|"
                           "chan|else|goto|package|switch|"
                           "const|fallthrough|if|range|type|"
                           "continue|for|import|return|var)"
                           "\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    word = rule.pattern.pattern();
    word.remove("\\b");
    word.remove("(");
    word.remove(")");
    foreach(QString v, word.split("|")) {
        allWords->insert(v);
    }

    //quotes and comment
    regexpQuotesAndComment = QRegExp("//|\\\"|'|`|/\\*");
}


bool GolangHighlighter::highlightPreBlock(QString const& text, int& startPos, int& endPos)
{
    int state = previousBlockState();

    if (state == -1)
        state = 0;

    if (state & STATE_BACKQUOTES) {
        endPos = findQuotesEndPos(text, startPos, '`');
        if (endPos == -1) {
            setFormat(0, text.length(), quotesFormat);
            setCurrentBlockState(STATE_BACKQUOTES);
            return true;
        } else {
            endPos += 1;
            setFormat(0, endPos - startPos, quotesFormat);
            startPos = endPos;
        }
    } else if (state & STATE_MULTILINE_COMMENT) {
        endPos = text.indexOf("*/", startPos);
        if (endPos == -1) {
            setFormat(0, text.length(), multiLineCommentFormat);
            setCurrentBlockState(previousBlockState());
            return true;
        } else {
            endPos += 2;
            setFormat(0, endPos - startPos, multiLineCommentFormat);
            startPos = endPos;
        }
    } else if (state & STATE_SINGLELINE_COMMENT) {
        setFormat(0, text.length(), singleLineCommentFormat);
        if (text.endsWith("\\")) {
            setCurrentBlockState(STATE_SINGLELINE_COMMENT);
        }
        return true;
    }
    return false;
}

void GolangHighlighter::highlightBlock(const QString &text)
{
    int startPos = 0;
    int endPos = text.length();

    setCurrentBlockState(0);

    if (highlightPreBlock(text, startPos, endPos)) {
        return;
    }

    //keyword and func
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text,startPos);
        while (index >= 0) {            
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            allWords->insert(text.mid(index,length));
            index = expression.indexIn(text, startPos+index + length);            
        }
    }

    //quote and comment
    while (true)
    {
        startPos = text.indexOf(regexpQuotesAndComment, startPos);
        if (startPos == -1)
            break;

        QString cap = regexpQuotesAndComment.cap();
        if ((cap == "\"") || (cap == "'") || (cap == "`"))
        {
            endPos = findQuotesEndPos(text, startPos + 1, cap.at(0));
            if (endPos == -1)
            {
                //multiline
                setFormat(startPos, text.length() - startPos, quotesFormat);
                if (cap == "`") {
                    setCurrentBlockState(STATE_BACKQUOTES);
                }
                return;
            }
            else
            {
                endPos += 1;
                setFormat(startPos, endPos - startPos, quotesFormat);
                startPos = endPos;
            }
        }
        else if (cap == "//")
        {
            setFormat(startPos, text.length() - startPos, singleLineCommentFormat);
            if (text.endsWith("\\"))
                setCurrentBlockState(STATE_SINGLELINE_COMMENT);
            return;
        }
        else if (cap == "/*")
        {
            endPos = text.indexOf("*/", startPos+2);
            if (endPos == -1)
            {
                //miltiline
                setFormat(startPos, text.length() - startPos, multiLineCommentFormat);
                setCurrentBlockState(STATE_MULTILINE_COMMENT);
                return;
            }
            else
            {
                endPos += 2;
                setFormat(startPos, endPos - startPos, multiLineCommentFormat);
                startPos = endPos;
            }
        }
    }
}

int GolangHighlighter::findQuotesEndPos(const QString &text, int startPos, const QChar &endChar)
{
    for (int pos = startPos; pos < text.length(); pos++)
    {        
        if (text.at(pos) == endChar) {
            return pos;
        } else if (text.at(pos) == QChar('\\') && endChar != QChar('`')) {
            ++pos;
        }
    }
    return -1;
}
