#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextDocument>

class QCheckBox;
 class QDialogButtonBox;
 class QGroupBox;
 class QLabel;
 class QLineEdit;
 class QPushButton;

 class FindDialog : public QDialog
 {
     Q_OBJECT

 public:
     FindDialog(QWidget *parent = 0);
 signals:
     void findText(const QString&,QTextDocument::FindFlags);
 private:
     QLabel *label;
     QLineEdit *lineEdit;
     QCheckBox *caseCheckBox;
     QCheckBox *fromStartCheckBox;
     QCheckBox *wholeWordsCheckBox;
     QCheckBox *searchSelectionCheckBox;
     QDialogButtonBox *buttonBox;
     QPushButton *findNextButton;
     QPushButton *findPrevButton;
 private slots:
     void findNextText();
     void findPrevText();
 };

#endif // FINDDIALOG_H
