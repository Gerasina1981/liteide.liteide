#ifndef GODOCPLUGIN_H
#define GODOCPLUGIN_H

#include "godoc_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"

class /*GODOCSHARED_EXPORT*/ GodocPlugin : public QObject, public IPlugin
{
    Q_OBJECT;
    Q_INTERFACES(IPlugin)
public:
    GodocPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual const PluginInfo & info() const;
};

#endif // GODOCPLUGIN_H
