#include "golanghighlighter.h"
#include <QDebug>

GolangHighlighter::GolangHighlighter(QTextDocument* document):
    QSyntaxHighlighter(document)
{
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    functionFormat.setForeground(Qt::blue);

    numberFormat.setForeground(Qt::darkMagenta);

    quotesFormat.setForeground(Qt::darkGreen);

    singleLineCommentFormat.setForeground(Qt::red);
    multiLineCommentFormat.setForeground(Qt::red);


    HighlightingRule rule;
    //keyword
    rule.pattern = QRegExp("\\b"
                           "(break|default|func|interface|select|"
                           "case|defer|go|map|struct|"
                           "chan|else|goto|package|struct|"
                           "const|fallthrough|if|range|byte|"
                           "continue|for|import|return|var|"
                           "nil|type)"
                           "\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    //number
    rule.pattern = QRegExp("(\\b|\\.)([0-9]+|0[xX][0-9a-fA-F]+|0[0-7]+)(\\.[0-9]+)?([eE][+-]?[0-9]+i?)?\\b");
    rule.format = numberFormat;

    highlightingRules.push_back(rule);

    //function
    rule.pattern = QRegExp("\\b[a-zA-Z_][a-zA-Z0-9_]+\\s*(?=\\()");
    rule.format = functionFormat;

    highlightingRules.push_back(rule);

    //quotes and comment
    regexpQuotesAndComment = QRegExp("//|\\\"|'|`|/\\*");
}


bool GolangHighlighter::highlightPreBlock(QString const& text, int& startPos, int& endPos)
{
    int state = previousBlockState();

    if (state == -1)
        state = 0;

    if ((state & STATE_QUOTES) || (state & STATE_BACKQUOTES)) {
        endPos = findQuotesEndPos(text, startPos, (state&STATE_BACKQUOTES) ? '`':'"');
        if (endPos == -1) {
            setFormat(0, text.length(), quotesFormat);
            setCurrentBlockState(STATE_QUOTES);
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
                setCurrentBlockState(cap.at(0) == QChar('`')? STATE_BACKQUOTES : STATE_QUOTES );
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
