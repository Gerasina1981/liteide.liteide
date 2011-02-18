#include "godocplugin.h"
#include "godocview.h"
#include <QtPlugin>


GodocPlugin::GodocPlugin()
{
}

void GodocPlugin::install(IApplication *app)
{
    new GodocView(app);
}

void GodocPlugin::uninstall()
{

}

const PluginInfo & GodocPlugin::info() const
{
    static PluginInfo s;
    s.name = "godoc";
    s.anchor = "visualfc";
    s.info = "Golang Doc";
    s.ver = "1.0";
    return s;
}

Q_EXPORT_PLUGIN(GodocPlugin)
