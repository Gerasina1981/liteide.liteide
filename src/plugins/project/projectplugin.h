#ifndef PROJECTPLUGIN_H
#define PROJECTPLUGIN_H

#include "project_global.h"
#include "../../api/iplugin.h"
#include "../../api/iapp.h"
#include "../../api/iproject.h"
#include "projectfile.h"
#include "projectmanager.h"

#include <QObject>
#include <QtPlugin>
#include <QAction>

class PROJECTSHARED_EXPORT ProjectPlugin : public QObject, public IPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
public:
    IProject *createProject(const QString &path, const QString &name);
    virtual void install(IApplication *app);
    virtual void uninstall();
    virtual QString name() const;
    virtual QString anchor() const;
    virtual QString info() const;
protected:
     ProjectManager *manager;
};

#endif // PROJECTPLUGIN_H
