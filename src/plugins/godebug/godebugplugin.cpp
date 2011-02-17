#include "godebugplugin.h"


GodebugPlugin::GodebugPlugin()
{
}

void GodebugPlugin::install(IApplication *app)
{
    liteApp = app;
    new GolangGdb(app);
}

void GodebugPlugin::uninstall()
{

}

const PluginInfo & GodebugPlugin::info() const
{
    static PluginInfo s;
    s.name = "godebug";
    s.anchor = "visualfc";
    s.info = "Golang Debug";
    s.ver = "1.0";
    return s;
}

Q_EXPORT_PLUGIN(GodebugPlugin)

