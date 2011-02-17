#include "golangoption.h"
#include "ui_golangoption.h"

#include <QFileDialog>
#include <QDebug>
#include <QDir>

GolangOption::GolangOption(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent),
    ui(new Ui::GolangOption)
{
    ui->setupUi(this);
}

GolangOption::~GolangOption()
{
    delete ui;
}

QWidget *GolangOption::widget()
{
    return this;
}

QIcon GolangOption::icon()
{
    return QIcon();
}

QString GolangOption::text()
{
    return "Golang";
}

void GolangOption::load()
{
    QString goroot = liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString();
    ui->lineEdit->setText(QDir::toNativeSeparators(goroot));
    setupList();
}

void GolangOption::save()
{
    liteApp->settings()->setValue("golang/GOROOT",ui->lineEdit->text());
}

void GolangOption::on_browseButton_clicked()
{
    QString goroot = QFileDialog::getExistingDirectory(this);
    if (!goroot.isEmpty()) {
        ui->lineEdit->setText(goroot);
        setupList();
    }
}

void GolangOption::setupList()
{
    QString root = ui->lineEdit->text();
}
