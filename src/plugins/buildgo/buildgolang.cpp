#include "buildgolang.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

BuildGolang::BuildGolang(QObject *parent) :
    QObject(parent)
{
    connect(&process8g,SIGNAL(outputText(QString,bool)),this,SLOT(outputText(QString)));
    connect(&process8l,SIGNAL(outputText(QString,bool)),this,SLOT(outputText(QString)));
    connect(&process8g,SIGNAL(processSuccess()),this,SLOT(linkProject()));
}

QString BuildGolang::buildName() const
{
    return tr("BuildGoalng");
}

bool BuildGolang::buildProject(IProject *proj)
{
    if (proj->values("GOFILES").isEmpty())
        return false;

    QStringList val = proj->values("TARGET");
    if (!val.isEmpty())
        target = val.at(0);
    else
        target = proj->displayName();

    target = QFileInfo(target).baseName();

    QString projDir = QFileInfo(proj->filePath()).absolutePath();
    process8l.setWorkingDirectory(projDir);
    process8g.setWorkingDirectory(projDir);
    QStringList args;
    args << "-o" << target+"_go_.8" << proj->values("GOFILES").join(" ");
    process8g.start(tr("8g build"),"c:\\go\\bin\\8g.exe",args);
    return true;
}

bool BuildGolang::buildEditor(IEditor *edit)
{
    return true;
}

void BuildGolang::outputText(const QString &text)
{
    qDebug() << text;
}

void BuildGolang::linkProject()
{
    QStringList args;
    args << "-o" << target+".exe" << target+"_go_.8";
    process8l.start(tr("8l link"),"c:\\go\\bin\\8l.exe",args);
}
