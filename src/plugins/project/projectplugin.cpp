#include "projectplugin.h"
#include "projectmanager.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>

void ProjectPlugin::install(IApplication *app)
{
    if (!app)
        return;
    manager = new ProjectManager(app);
}

void ProjectPlugin::uninstall()
{
}

QString ProjectPlugin::name() const
{
    return "Project";
}

QString ProjectPlugin::anchor() const
{
    return "visualfc";
}

QString ProjectPlugin::info() const
{
    return tr("Project Manager 1.0");
}

Q_EXPORT_PLUGIN(ProjectPlugin)
