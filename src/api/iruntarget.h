#ifndef __LITEAPI_IRUNTARGET_H__
#define __LITEAPI_IRUNTARGET_H__

#include <QString>

class IProject;
class IEditor;

class IRunTarget
{
public:
    virtual  ~IRunTarget() {}
    virtual void runProject(IProject *proj) = 0;
    virtual void runEditor(IEditor *edit) = 0;
};

class IRunTargetEvent
{
public:
    virtual ~IRunTargetEvent() {}
    virtual void fireRunTargetStarted() = 0;
    virtual void fireRunTargetStoped(bool success) = 0;
    virtual void fireRunTargetOutput(const QByteArray &text, bool stdError) = 0;
};

#endif // __LITEAPI_IRUNTARGET_H__
