#ifndef GOASTVIEWPLUGIN_H
#define GOASTVIEWPLUGIN_H

#include "goastview_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/ieditor.h"

#include <QObject>
#include <QtPlugin>

class /*GOASTVIEWSHARED_EXPORT*/ GoAstViewPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    GoAstViewPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString anchor() const;
    virtual QString info() const;
private:
    IApplication *liteApp;
};

#endif // GOASTVIEWPLUGIN_H
