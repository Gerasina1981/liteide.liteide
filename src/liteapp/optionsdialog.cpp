#include "optionsdialog.h"
#include "../api/ioption.h"

#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QPushButton>

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent)
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::ListMode);
    //contentsWidget->setIconSize(QSize(32, 32));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(4);

    pagesWidget = new QStackedWidget;
    QPushButton *saveButton = new QPushButton(tr("OK"));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    QPushButton *applyButton = new QPushButton(tr("Apply"));

    //createIcons();
    //contentsWidget->setCurrentRow(0);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(saveButton,SIGNAL(clicked()),this,SLOT(closeAndSave()));
    connect(applyButton,SIGNAL(clicked()),this,SLOT(apply()));
    //connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(applyButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    //mainLayout->addStretch(1);
    //mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("LiteIDE Options"));

    connect(contentsWidget,
                 SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
                 this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void OptionsDialog::addPage(IOption *opt)
{
    QListWidgetItem *btn = new QListWidgetItem(contentsWidget);
    btn->setIcon(opt->icon());
    btn->setText(opt->text());
    btn->setTextAlignment(Qt::AlignLeft);// | Qt::AlignHCenter);
    btn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    pagesWidget->addWidget(opt->widget());

    options.append(opt);
}

void OptionsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void OptionsDialog::closeAndSave()
{
    apply();
    close();
}

void OptionsDialog::apply()
{
    foreach(IOption *opt, options) {
        opt->save();
    }
}

void OptionsDialog::show()
{
    foreach(IOption *opt, options) {
        opt->load();
    }
    QDialog::show();
}
