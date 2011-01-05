#include <QtGui>

#include "syntaxeditor.h"
#include "golanghighlighter.h"

SyntaxEditor::SyntaxEditor()
{
    setAttribute(Qt::WA_DeleteOnClose);
    setLineWrapMode(QPlainTextEdit::NoWrap);

    isUntitled = true;
    editorArea = new SyntaxEditorArea(this);
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateEditorArea(QRect,int)));

    updateAreaWidth(0);
}

void SyntaxEditor::reload()
{
    QFile file(curFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(curFile)
                             .arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    setPlainText(codec->toUnicode(file.readAll()));
    /*
    QTextCursor cursor(document());
    cursor.beginEditBlock();
    cursor.select(QTextCursor::Document);
    cursor.insertText(codec->toUnicode(file.readAll()));
    cursor.endEditBlock();
    */
    QApplication::restoreOverrideCursor();
}

void SyntaxEditor::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.go").arg(sequenceNumber++);
    curText = curFile + "[*]";
    setWindowTitle(curText);
}

SyntaxEditor *SyntaxEditor::openFile(const QString &fileName)
{
    SyntaxEditor *editor = new SyntaxEditor;
    if (editor->loadFile(fileName)) {
        editor->setCurrentFile(fileName);
        return editor;
    } else {
        delete editor;
        return 0;
    }
}

bool SyntaxEditor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    setPlainText(codec->toUnicode(file.readAll()));
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    return true;
}

bool SyntaxEditor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool SyntaxEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool SyntaxEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LiteIDE"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    file.write(codec->fromUnicode(toPlainText()));
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    return true;
}

QString SyntaxEditor::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void SyntaxEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool SyntaxEditor::maybeSave()
{
    if (document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("LiteIDE"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
                     | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void SyntaxEditor::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    curText = userFriendlyCurrentFile();
}

QString SyntaxEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

int SyntaxEditor::editorAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void SyntaxEditor::areaPaintEvent(QPaintEvent *event)
{
    QPainter painter(editorArea);

    painter.fillRect(event->rect(), Qt::lightGray);//lightGray);

//![extraAreaPaintEvent_0]

//![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
//![extraAreaPaintEvent_1]

//![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, editorArea->width()-3, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void SyntaxEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    editorArea->setGeometry(QRect(cr.left(), cr.top(), editorAreaWidth(), cr.height()));
}

void SyntaxEditor::updateAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(editorAreaWidth(), 0, 0, 0);
}

void SyntaxEditor::updateEditorArea(const QRect &rect, int dy)
{
    if (dy)
        editorArea->scroll(0, dy);
    else
        editorArea->update(0, rect.y(), editorArea->width(), rect.height());

    if (rect.contains(viewport()->rect())) {
        updateAreaWidth(0);
    }
}

bool SyntaxEditor::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return ||
            keyEvent->key() == Qt::Key_Enter) {

            QTextCursor cursor(this->textCursor());

            cursor.beginEditBlock();
            int pos = cursor.positionInBlock();
            QString text = cursor.block().text();
            int i = 0;
            int tab = 0;
            int space = 0;
            QString inText = "\n";
            while (i < text.size()) {
                if (!text.at(i).isSpace())
                    break;
                if (text.at(0) == ' ') {
                    space++;
                }
                else if (text.at(0) == '\t') {
                    inText += "\t";
                    tab++;
                }
                i++;
            }
            text.trimmed();
            if (!text.isEmpty()) {
                if (pos >= text.size()) {
                    const QChar ch = text.at(text.size()-1);
                    if (ch == '{' || ch == '(') {
                        inText += "\t";
                    }
                } else if (pos == text.size()-1 && text.size() >= 3) {
                    qDebug() << "center";
                    const QChar l = text.at(text.size()-2);
                    const QChar r = text.at(text.size()-1);
                    if ( (l == '{' && r == '}') ||
                         (l == '(' && r== ')') ) {
                        cursor.insertText(inText);
                        int pos = cursor.position();
                        cursor.insertText(inText);
                        cursor.setPosition(pos);
                        this->setTextCursor(cursor);
                        cursor.insertText("\t");
                        cursor.endEditBlock();
                        return true;
                    }
                }
            }
            cursor.insertText(inText);
            cursor.endEditBlock();
            return true;
        } else if (keyEvent->key() == '(') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("()");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == '{') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("{}");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == '\"') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("\"\"");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == '\`') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("\`\`");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        } else if (keyEvent->key() == '\'') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("\'\'");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        }
        /*
        else if (keyEvent->key() == Qt::Key_Tab) {
            //qDebug() << this->cursorForPosition()
            QTextCursor cursor(this->textCursor());
            if (cursor.hasSelection()) {
                QTextBlock block = document()->findBlock(cursor.selectionStart());
                const QTextBlock end = document()->findBlock(cursor.selectionEnd()).next();
                QTextCursor tc = textCursor();
                tc.beginEditBlock();
                do {
                    int indent;
                    int padding;
                  //  codeFormatter.indentFor(block, &indent, &padding);
                  //  ts.indentLine(block, indent + padding, padding);
                  //  codeFormatter.updateLineStateChange(block);
                    QTextCursor cur(block);
                    cur.movePosition(QTextCursor::StartOfLine);//,QTextCursor::KeepAnchor);
                    cur.insertText("\t");
                    block = block.next();
                } while (block.isValid() && block != end);
                tc.endEditBlock();
            }
            return true;
        }
        */
    }
    return QPlainTextEdit::event(event);
}
