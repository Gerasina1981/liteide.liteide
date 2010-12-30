#ifndef __LITEIDE_IOUTPUT_H__
#define __LITEIDE_IOUTPUT_H__

#include <QWidget>

class IOutput : public QObject
{
    Q_OBJECT
public:
    IOutput(QObject *parent = 0) : QObject(parent) {}
    virtual ~IOutput() {}
    virtual QWidget *widget() = 0;
    virtual void clear() = 0;
public slots:
    virtual void outputText(const QString &text) = 0;
};

class IOutputEvent
{
public:
    virtual  ~IOutputEvent() {}
    virtual void fireOutputChanged(IOutput *output) = 0;
};

#endif // __LITEIDE_IOUTPUT_H__
