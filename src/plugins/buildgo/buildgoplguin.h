#ifndef BUILDGOPLGUIN_H
#define BUILDGOPLGUIN_H

#include "buildgo_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "buildgolang.h"

#include <QObject>
#include <QtPlugin>

class BUILDGOSHARED_EXPORT BuildgoPlguin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    BuildgoPlguin();
    virtual ~BuildgoPlguin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString info() const;
private:
    BuildGolang *build;
};

#endif // BUILDGOPLGUIN_H
