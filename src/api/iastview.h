#ifndef __LITEAPI_IASTVIEW_H__
#define __LITEAPI_IASTVIEW_H__

#include <QObject>
#include <QString>
#include <QByteArray>

class IEditor;

class IAstView
{
public:
    virtual ~IAstView() {}
    virtual void update(const QString &fileName, const QByteArray &data) = 0;
};

class IAstViewFactory
{
public:
    virtual ~IAstViewFactory() {}
    virtual QStringList fileTypes() = 0;
    virtual IAstView *load(const QString &fileName, const QByteArray &data) = 0;
};

#endif // __LITEAPI_IASTVIEW_H__
