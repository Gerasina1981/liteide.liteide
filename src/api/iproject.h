#ifndef __LITEAPI_IPROJECT_H__
#define __LITEAPI_IPROJECT_H__

#include <QString>
#include <QStringList>

class IProject
{
public:
    virtual ~IProject() {}
    virtual QString displayName() const = 0;
    virtual QString filePath() const = 0;
    virtual QString fileName() const = 0;
    virtual QStringList values(const QString &key) = 0;
    virtual bool reload() = 0;
    virtual bool open(const QString &filePath) = 0;    
    virtual void close() = 0;
    virtual bool isMakefile() = 0;
};

class IProjectFactory
{
public:
    virtual ~IProjectFactory() {}
    virtual QStringList projectKeys() const = 0;
    virtual QStringList openTypeFilter() const = 0;
    virtual QStringList openTypeFilterList() const = 0;
    virtual IProject *loadProject(const QString &filePath) = 0;
};

class IProjectEvent
{
public:
    virtual ~IProjectEvent() {}
    virtual void fireProjectChanged(IProject *project) = 0;
    virtual void fireProjectClose(IProject *project) = 0;
};

#endif // __LITEAPI_IPROJECT_H__
