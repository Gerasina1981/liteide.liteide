#include <QtGui/QApplication>
#include <QFileInfo>
#include <QDir>
#include "liteapp.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LiteApp app;

    QFileInfo info;
    info.setFile(QDir(a.applicationDirPath()),"plugins");
    app.loadPlugins(info.absoluteFilePath());
    app.installPlugins();

    app.mainWindow->show();

    return a.exec();
}
