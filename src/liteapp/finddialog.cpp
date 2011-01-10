#include <QtGui>
#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
     : QDialog(parent)
 {
     label = new QLabel(tr("Find &what:"));
     lineEdit = new QLineEdit;
     label->setBuddy(lineEdit);

     caseCheckBox = new QCheckBox(tr("Match &case"));
     fromStartCheckBox = new QCheckBox(tr("Search from &start"));
     fromStartCheckBox->setChecked(true);

     findNextButton = new QPushButton(tr("&Find Next"));
     findNextButton->setDefault(true);
     connect(findNextButton,SIGNAL(clicked()),this,SLOT(findNextText()));

     findPrevButton = new QPushButton(tr("Find &Prev"));
     findPrevButton->setDefault(true);
     connect(findPrevButton,SIGNAL(clicked()),this,SLOT(findPrevText()));


     buttonBox = new QDialogButtonBox(Qt::Vertical);
     buttonBox->addButton(findNextButton, QDialogButtonBox::ActionRole);
     buttonBox->addButton(findPrevButton, QDialogButtonBox::ActionRole);

     wholeWordsCheckBox = new QCheckBox(tr("&Whole words"));
     searchSelectionCheckBox = new QCheckBox(tr("Search se&lection"));


     QHBoxLayout *topLeftLayout = new QHBoxLayout;
     topLeftLayout->addWidget(label);
     topLeftLayout->addWidget(lineEdit);

     QVBoxLayout *leftLayout = new QVBoxLayout;
     leftLayout->addLayout(topLeftLayout);
     leftLayout->addWidget(caseCheckBox);
     leftLayout->addWidget(wholeWordsCheckBox);
     leftLayout->addStretch(1);

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->setSizeConstraint(QLayout::SetFixedSize);
     mainLayout->addLayout(leftLayout, 0, 0);
     mainLayout->addWidget(buttonBox, 0, 1);
     setLayout(mainLayout);
 }

void FindDialog::findNextText()
{
    QString text = lineEdit->text();
    text.trimmed();
    if (text.isEmpty())
        return;

    QTextDocument::FindFlags flags = 0;
    if (wholeWordsCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }
    if (caseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    emit findText(text,flags);
}

void FindDialog::findPrevText()
{
    QString text = lineEdit->text();
    text.trimmed();
    if (text.isEmpty())
        return;

    QTextDocument::FindFlags flags =  QTextDocument::FindBackward;

    if (wholeWordsCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }

    if (caseCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    emit findText(text,flags);
}

