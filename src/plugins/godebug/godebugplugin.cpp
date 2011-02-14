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

QString GodebugPlugin::name() const
{
    return "GoDebug";
}

QString GodebugPlugin::anchor() const
{
    return "visualfc";
}

QString GodebugPlugin::info() const
{
    return tr("Golang Debug 1.0");
}

Q_EXPORT_PLUGIN(GodebugPlugin)

