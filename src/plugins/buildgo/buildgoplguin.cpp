#include "buildgoplguin.h"
#include "buildgolang.h"

BuildgoPlguin::BuildgoPlguin()
{
}

void BuildgoPlguin::install(IApplication *app)
{
    app->addBuild(new BuildGolang);
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

