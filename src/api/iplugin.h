#ifndef __LITEAPI_IPLUGIN_H__
#define __LITEAPI_IPLUGIN_H__

#include <QObject>

class IApplication;
class IPlugin
{
public:
    virtual ~IPlugin() {}
    virtual void install(IApplication *app) = 0;
    virtual void uninstall() = 0;
    virtual QString name() const = 0;
    virtual QString info() const = 0;
};

Q_DECLARE_INTERFACE(IPlugin,"lite.IPlugin/1.0")

#endif //__LITEAPI_IPLUGIN_H__
