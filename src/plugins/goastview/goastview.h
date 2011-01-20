#ifndef GOASTVIEW_H
#define GOASTVIEW_H

#include <QObject>
#include "../../api/iastview.h"

class GoAstView : public QObject, public IAstView
{
    Q_OBJECT
public:
    explicit GoAstView(QObject *parent = 0);
    virtual void update(const QString &fileName, const QByteArray &data);
signals:

public slots:

};

#endif // GOASTVIEW_H
