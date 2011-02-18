#ifndef GODOCVIEW_H
#define GODOCVIEW_H

#include <QWidget>
#include <QDockWidget>
#include "../../api/iapp.h"

class QTextBrowser;
class GodocView : public QWidget
{
    Q_OBJECT
public:
    explicit GodocView(IApplication *app, QWidget *parent = 0);
protected:
    IApplication *liteApp;
    QDockWidget   *dock;
    QTextBrowser  *browser;
signals:

public slots:

};

#endif // GODOCVIEW_H
