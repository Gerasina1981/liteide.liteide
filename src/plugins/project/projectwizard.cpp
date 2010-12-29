#include "projectwizard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QFileDialog>
#include <QVariant>
#include <QFileInfo>
#include <QMessageBox>

ProjectWizard::ProjectWizard(QWidget *parent) :
    QWizard(parent)
{
    this->addPage(new ProjectInfoPage);
    setWindowTitle(tr("Project Wizard"));
}


ProjectInfoPage::ProjectInfoPage(QWidget *parent) :
        QWizardPage(parent)
{
    setTitle(tr("Project Information"));
    setSubTitle(tr("This wizard generates application project."));

    nameLineEdit = new QLineEdit;
    locationLineEdit = new QLineEdit;
    QPushButton *browseButton = new QPushButton(tr("Browse..."));
    connect(browseButton,SIGNAL(clicked()),this,SLOT(browseLocation()));

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(new QLabel(tr("Name:")),nameLineEdit);

    QHBoxLayout *locationLayout = new QHBoxLayout;
    locationLayout->addWidget(locationLineEdit);
    locationLayout->addWidget(browseButton);

    formLayout->addRow(new QLabel("Location:"),locationLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addLayout(formLayout);


    registerField("projectName*",nameLineEdit);
    registerField("projectLocation*",locationLineEdit);

    setLayout(mainLayout);
}

void ProjectInfoPage::browseLocation()
{
    QString location = QFileDialog::getExistingDirectory(this);
    if (!location.isEmpty()) {
        locationLineEdit->setText(location);
    }
}

void ProjectWizard::accept()
{
    projectName = field("projectName").toByteArray();
    projectLocation = field("projectLocation").toByteArray();

    if (!QFileInfo(projectLocation).isWritable()) {
        QMessageBox::information(this,tr("Error"),
                                 tr("Project Location {%1} cannot write!").arg(projectLocation));
        return;
    }
    if (QFileInfo(QDir(projectLocation),projectName).isReadable()) {
        QMessageBox::information(this,tr("Error"),
                                 tr("Project exists {%1}!").arg(projectLocation));
        return;
    }

    QWizard::accept();
}
