#include "projecthighlighter.h"

ProjectHighlighter::ProjectHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
    keywordFormat.setForeground(Qt::darkBlue);

    HighlightingRule rule;
    //keyword
    rule.pattern = QRegExp("\\b"
                           "(TARGET|DESTDIR|GOFILES|CGOFILES)"
                           "\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);
}

void ProjectHighlighter::highlightBlock(const QString &text)
{
    int startPos = 0;
    //int endPos = text.length();
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text,startPos);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, startPos+index + length);
        }
    }
}
