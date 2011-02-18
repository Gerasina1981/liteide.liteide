#include "godocview.h"
#include <QTextBrowser>
#include <QVBoxLayout>

GodocView::GodocView(IApplication *app, QWidget *parent) :
    liteApp(app), QWidget(parent)
{
    dock = liteApp->mainWindow()->addWorkspacePane(this,tr("Godoc"));
    dock->setAllowedAreas(Qt::RightDockWidgetArea);
    browser = new QTextBrowser;
    QVBoxLayout *main = new QVBoxLayout;
    main->addWidget(browser);
    this->setLayout(main);
}
