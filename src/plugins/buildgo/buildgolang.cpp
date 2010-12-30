#include "buildgolang.h"

BuildGolang::BuildGolang(QObject *parent) :
    QObject(parent)
{
}

QString BuildGolang::buildName() const
{
    return tr("BuildGoalng");
}

bool BuildGolang::buildProject(IProject *proj)
{
    return true;
}

bool BuildGolang::buildEditor(IEditor *edit)
{
    return true;
}
