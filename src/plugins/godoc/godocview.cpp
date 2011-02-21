#include "godocview.h"
#include <QTextBrowser>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include "../golangpro/golangoption.h"

GodocView::GodocView(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent)
{
    dock = liteApp->mainWindow()->addWorkspacePane(this,tr("Godoc"));
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    browser = new QTextBrowser;
    line = new QLineEdit;
    QHBoxLayout *top = new QHBoxLayout;
    top->addWidget(line);
    QPushButton *btn = new QPushButton("Search");
    connect(btn,SIGNAL(clicked()),this,SLOT(packageDoc()));
    //btn->setSizePolicy(Qt::PreferredSize);
    top->addWidget(btn);
    QVBoxLayout *main = new QVBoxLayout;
    main->addLayout(top);
    main->addWidget(browser);
    main->setMargin(1);
    this->setLayout(main);

    connect(&process,SIGNAL(readyReadStandardOutput()),this,SLOT(readStdout()));
    connect(&process,SIGNAL(readyReadStandardError()),this,SLOT(readStderr()));
    connect(&process,SIGNAL(started()),this,SLOT(started()));
    connect(&process,SIGNAL(finished(int)),this,SLOT(finished(int)));
    connect(&process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(error(QProcess::ProcessError)));

}

void GodocView::packageDoc()
{
    QString pack = line->text();
    TargetInfo info = liteApp->getTargetInfo();
    process.setWorkingDirectory(info.workDir);
    QString goroot = liteApp->settings()->value("golang/GOROOT",defGOROOT()).toString();
    QString godoc = QFileInfo(QDir(goroot),"bin/godoc"+liteApp->osExecuteExt()).absoluteFilePath();
    qDebug() << goroot << godoc;
    QStringList args;
    args << "-html=true" << pack ;
    process.start(godoc,args);
}

void GodocView::finished(int code)
{
}

void GodocView::error(QProcess::ProcessError code)
{
//    appendBuildOutput("Build error : "+processErrorName(code),true);
    qDebug() << "error";
}

void GodocView::readStdout()
{
    QByteArray text = process.readAllStandardOutput();
    browser->insertHtml(text);
}
void GodocView::readStderr()
{
    QByteArray text = process.readAllStandardError();
    //browser->setText(text);
    qDebug() << text;
}

void GodocView::started()
{
    qDebug() << "start";
}
