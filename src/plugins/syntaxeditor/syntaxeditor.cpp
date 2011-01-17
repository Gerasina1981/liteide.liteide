#include <QtGui>

#include "syntaxeditor.h"
#include "golanghighlighter.h"
#include "QCompleter"

SyntaxEditor::SyntaxEditor() : editCompleter(0)
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
        if (this->autoIndent &&
            ( keyEvent->key() == Qt::Key_Return ||
              keyEvent->key() == Qt::Key_Enter) ) {

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
        } else if (this->autoBlock && keyEvent->key() == '{') {
            QTextCursor cursor(this->textCursor());
            cursor.insertText("{}");
            cursor.movePosition(QTextCursor::PreviousCharacter);
            setTextCursor(cursor);
            return true;
        }
    }
    return QPlainTextEdit::event(event);
}

void SyntaxEditor::setCompleter(SyntaxCompleter *completer)
{
    if (editCompleter)
        QObject::disconnect(editCompleter, 0, this, 0);

    editCompleter = completer;

    if (!editCompleter)
        return;

    editCompleter->setFileName(this->curFile);
    editCompleter->setWidget(this);
    editCompleter->setCompletionMode(QCompleter::PopupCompletion);
    editCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(editCompleter, SIGNAL(activated(QString)),
                     this, SLOT(insertCompletion(QString)));
}

SyntaxCompleter *SyntaxEditor::completer() const
{
    return this->editCompleter;
}


QString SyntaxEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void SyntaxEditor::focusInEvent(QFocusEvent *e)
{
    if (editCompleter)
         editCompleter->setWidget(this);
     QPlainTextEdit::focusInEvent(e);
}

void SyntaxEditor::keyPressEvent(QKeyEvent *e)
{
    if (editCompleter && editCompleter->popup()->isVisible()) {
          // The following keys are forwarded by the completer to the widget
         switch (e->key()) {
         case Qt::Key_Enter:
         case Qt::Key_Return:
         case Qt::Key_Escape:
         case Qt::Key_Tab:
         case Qt::Key_Backtab:
              e->ignore();
              return; // let the completer do default behavior
         default:
             break;
         }
      }

      bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
      if (!editCompleter || !isShortcut) // do not process the shortcut when we have a completer
          QPlainTextEdit::keyPressEvent(e);

      const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
      if (!editCompleter || (ctrlOrShift && e->text().isEmpty()))
          return;

      static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
      bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
      QString completionPrefix = textUnderCursor();

      editCompleter->underCursor(this->textCursor(),completionPrefix);

      if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                        || eow.contains(e->text().right(1)))) {
          editCompleter->popup()->hide();
          return;
      }

      if (completionPrefix != editCompleter->completionPrefix()) {
          editCompleter->setCompletionPrefix(completionPrefix);
          editCompleter->popup()->setCurrentIndex(editCompleter->completionModel()->index(0, 0));
      }
      QRect cr = cursorRect();
      cr.setWidth(editCompleter->popup()->sizeHintForColumn(0)
                  + editCompleter->popup()->verticalScrollBar()->sizeHint().width());
      editCompleter->complete(cr); // popup it up!
}

void SyntaxEditor::insertCompletion(const QString& completion)
{
    if (editCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - editCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}
