#ifndef __LITEAPI_IPROJECT_H__
#define __LITEAPI_IPROJECT_H__

#include <QObject>

class IProject
{
public:
    virtual ~IProject() {}
    virtual QString name() const = 0;
    virtual QString fullPath() const = 0;
    virtual QStringList files() const = 0;
};

#endif //__LITEAPI_IPROJECT_H__
