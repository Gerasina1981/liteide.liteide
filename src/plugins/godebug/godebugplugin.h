#ifndef GODEBUGPLUGIN_H
#define GODEBUGPLUGIN_H

#include <QtPlugin>
#include "godebug_global.h"

#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/iproject.h"
#include "golanggdb.h"

class /*GODEBUGSHARED_EXPORT */GodebugPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    GodebugPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString anchor() const;
    virtual QString info() const;
protected:
    IApplication *liteApp;
 };

#endif // GODEBUGPLUGIN_H
