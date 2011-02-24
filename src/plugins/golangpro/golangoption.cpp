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
    ui->gorootEdit->setText(QDir::toNativeSeparators(goroot));
    QString gdb = liteApp->settings()->value("golang/gdb","gdb").toString();
    ui->gdbEdit->setText(QDir::toNativeSeparators(gdb));
    QString make = liteApp->settings()->value("golang/make",goroot+"/bin/gomake").toString();
    ui->makeEdit->setText(QDir::toNativeSeparators(make));
    bool useGomake = liteApp->settings()->value("golang/usegomake",true).toBool();
    ui->useGoMakeCheck->setChecked(useGomake);
    setupList();
}

void GolangOption::save()
{
    liteApp->settings()->setValue("golang/GOROOT",ui->gorootEdit->text());
    liteApp->settings()->setValue("golang/gdb",ui->gdbEdit->text());
    liteApp->settings()->setValue("golang/make",ui->makeEdit->text());
    liteApp->settings()->setValue("golang/usegomake",ui->useGoMakeCheck->isChecked());
}

void GolangOption::setupList()
{
}

void GolangOption::on_browseGoroot_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this);
    if (!dir.isEmpty()) {
        ui->gorootEdit->setText(dir);
        setupList();
    }
}

void GolangOption::on_browseGdb_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this);
    if (!dir.isEmpty()) {
        ui->gdbEdit->setText(dir);
        setupList();
    }
}

void GolangOption::on_browseMake_clicked()
{
    QString dir = QFileDialog::getOpenFileName(this);
    if (!dir.isEmpty()) {
        ui->makeEdit->setText(dir);
        setupList();
    }
}

void GolangOption::on_useGoMakeCheck_toggled(bool checked)
{
    ui->makeEdit->setEnabled(!checked);
    ui->browseMake->setEnabled(!checked);
}
