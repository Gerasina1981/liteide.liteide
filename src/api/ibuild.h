#ifndef __LITEIDE_IBUILD_H_
#define __LITEIDE_IBUILD_H_

#include <QString>

class IProject;
class IEditor;

class IBuild
{
public:
    virtual ~IBuild() {}
    virtual QString buildName() const = 0;
    virtual void setActive() = 0;
    virtual bool buildProject(IProject *proj) = 0;
    virtual bool buildFile(const QString &fileName) = 0;
    virtual void cancelBuild() = 0;
};

class IBuildEvent
{
public:
    virtual ~IBuildEvent() {}
    virtual void fireBuildStarted() = 0;
    virtual void fireBuildStoped(bool success) = 0;
    virtual void fireBuildOutput(const QString &text, bool stdError) = 0;
};


#endif // __LITEIDE_IBUILD_H_
