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
    return tr("Buildgo");
}

QString BuildgoPlguin::info() const
{
    return tr("Buildgo");
}

Q_EXPORT_PLUGIN(BuildgoPlguin)

