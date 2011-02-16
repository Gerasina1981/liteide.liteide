#ifndef IOPTION_H
#define IOPTION_H

#include <QWidget>

class IOption
{
public:
    virtual ~IOption() {}
    virtual QWidget *widget() = 0;
    virtual QIcon icon() = 0;
    virtual QString text() = 0;
    virtual void load() = 0;
    virtual void save() = 0;
};


#endif // IOPTION_H
