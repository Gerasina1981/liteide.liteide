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
    virtual bool buildProject(IProject *proj) = 0;
    virtual bool buildEditor(IEditor *edit) = 0;
};

#endif // __LITEIDE_IBUILD_H_
