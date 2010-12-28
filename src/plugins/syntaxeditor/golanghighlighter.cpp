#include "golanghighlighter.h"
#include <QDebug>

GolangHighlighter::GolangHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bbreak\\b" << "\\bdefault\\b" << "\\bfunc\\b" << "\\binterface\\b" << "\\bselect\\b"
                    << "\\bcase\\b"   << "\\bdefer\\b" << "\\bgo\\b" << "\\bmap\\b" << "\\bstruct\\b"
                    << "\\bchan\\b" << "\\belse\\b"   << "\\bgoto\\b" << "\\bpackage\\b" << "\\bswitch\\b"
                    << "\\bconst\\b" << "\\bfallthrough\\b" << "\\bif\\b" << "\\brange\\b" << "\\btype\\b"
                    << "\\bcontinue\\b" << "\\bfor\\b" << "\\bimport\\b" << "\\breturn\\b" << "\\bvar\\b"
                    << "\\bnil\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
//! [0] //! [1]
    }
//! [1]

//! [2]
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]

//! [3]
    singleLineCommentFormat.setForeground(Qt::red);
    singleLineCommentExpression = QRegExp("//[^\n]*");
    /*
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
    */
    multiLineCommentFormat.setForeground(Qt::red);
//! [3]

//! [4]
    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);
//! [4]

//! [5]
    //functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_.]+(?=\\()");
    rule.format = functionFormat;

    highlightingRules.append(rule);
//! [5]

//! [6]
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}
//! [6]

//! [7]
void GolangHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int singleIndex = singleLineCommentExpression.indexIn(text);
    if (singleIndex >= 0 && previousBlockState() <= 0) {
        int index = 0;
        while (singleIndex >= 0 && previousBlockState() <= 0) {
            int startIndex = commentStartExpression.indexIn(text,index);
            int endIndex = commentEndExpression.indexIn(text, startIndex);
            int length = 0;
            bool bformat = false;
            if (startIndex < 0 ) {
                bformat = true;
            } else if (startIndex >= 0 && singleIndex < startIndex) {
                bformat = true;
            } else if (startIndex >=0 && endIndex>= 0 &&
                       singleIndex > endIndex ) {
                bformat = true;
            }
            if (startIndex >=0 && endIndex >= 0) {
                setFormat(startIndex, endIndex-startIndex+commentEndExpression.matchedLength(), multiLineCommentFormat);
            }

            if (bformat) {
                length = singleLineCommentExpression.matchedLength();
                setFormat(singleIndex, length, singleLineCommentFormat);
            } else if (endIndex >= 0) {
                length = commentEndExpression.matchedLength();
            } else if (startIndex >= 0) {
                length = commentStartExpression.matchedLength();
            }
            index = singleIndex + length;
            singleIndex = singleLineCommentExpression.indexIn(text,index);
        }
        return;
    }

//! [7] //! [8]
//! [8]

//! [9]

    int startIndex = 0;
    if (previousBlockState() != 1) {
         startIndex = commentStartExpression.indexIn(text);
    }

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
