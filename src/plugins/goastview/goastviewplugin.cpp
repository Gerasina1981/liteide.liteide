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

QString GoAstViewPlugin::name() const
{
    return "GoAstView";
}

QString GoAstViewPlugin::anchor() const
{
    return "visualfc";
}

QString GoAstViewPlugin::info() const
{
    return tr("Golang Ast View 1.0");
}

Q_EXPORT_PLUGIN(GoAstViewPlugin)
