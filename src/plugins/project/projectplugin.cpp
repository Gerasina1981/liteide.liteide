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

const PluginInfo & ProjectPlugin::info() const
{
    static PluginInfo s;
    s.name = "project";
    s.anchor = "visualfc";
    s.info = tr("Project Manager");
    s.ver = tr("0.2.1");
    return s;
}

Q_EXPORT_PLUGIN(ProjectPlugin)
