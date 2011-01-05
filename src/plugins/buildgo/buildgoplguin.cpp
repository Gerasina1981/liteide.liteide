#include "buildgoplguin.h"
#include "buildgolang.h"

#include <QDebug>

BuildgoPlguin::BuildgoPlguin() : build(NULL)
{
}

BuildgoPlguin::~BuildgoPlguin()
{
    if (build) {
        delete build;
    }
}


void BuildgoPlguin::install(IApplication *app)
{
    build = new BuildGolang(app);
    app->addBuild(build);
}

void BuildgoPlguin::uninstall()
{

}

QString BuildgoPlguin::name() const
{
    return "BuildGo";
}

QString BuildgoPlguin::anchor() const
{
    return "visualfc";
}

QString BuildgoPlguin::info() const
{
    return tr("Build Golang by gopromake 1.0");
}

Q_EXPORT_PLUGIN(BuildgoPlguin)

