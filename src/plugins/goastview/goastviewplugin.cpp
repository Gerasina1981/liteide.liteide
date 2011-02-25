#include "goastviewplugin.h"
#include "goastviewmanager.h"


GoAstViewPlugin::GoAstViewPlugin() : liteApp(NULL)
{
}

void GoAstViewPlugin::install(IApplication *app)
{
    liteApp = app;
    new GoAstViewManager(app);
}

void GoAstViewPlugin::uninstall()
{

}

const PluginInfo & GoAstViewPlugin::info() const
{
    static PluginInfo s;
    s.name = "goastview";
    s.anchor = "visualfc";
    s.info = "Golang AstView";
    s.ver = "0.2.1";
    return s;
}

Q_EXPORT_PLUGIN(GoAstViewPlugin)
