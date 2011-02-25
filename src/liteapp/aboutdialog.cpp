#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->devLineEdit->setText("visualfc (visualfc@gmail.com)");
    ui->projectLineEdit->setText("http://code.google.com/p/golangide");
    ui->verLineEdit->setText("0.2.1");
    ui->buildLineEdit->setText(__DATE__);
    ui->thankLineEdit->setText("Lucy and Mickey");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
