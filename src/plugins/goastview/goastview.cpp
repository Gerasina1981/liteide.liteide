#include "goastview.h"

#include <QDebug>
#include <QProcess>

GoAstView::GoAstView(QObject *parent) :
    QObject(parent)
{
}

void GoAstView::update(const QString &fileName, const QByteArray &data)
{
    qDebug() << fileName;
}
