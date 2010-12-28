#ifndef GOLANGPROPLUGIN_H
#define GOLANGPROPLUGIN_H

#include "golangpro_global.h"
#include "../../liteapi/liteapi.h"

#include <QObject>
#include <QtPlugin>

class GOLANGPROSHARED_EXPORT GolangProPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    GolangProPlugin();
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString info() const;    
private slots:
    void gofmt();
protected:
    IApplication *liteApp;
    QAction *gofmtAct;
    QString gofmtCmd;
    QString go8gCmd;
    QString go8lCmd;
};

#endif // GOLANGPROPLUGIN_H
