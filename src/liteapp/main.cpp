#include <QtGui/QApplication>
#include <QFileInfo>
#include <QDir>
#include "liteapp.h"
#include <QDebug>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LiteApp app;

    QFileInfo info;
    info.setFile(QDir(a.applicationDirPath()),"plugins");
    app.loadPlugins(info.absoluteFilePath());
    app.installPlugins();

    app.mainWindow->show();

    if (argc >= 2) {
        //app.loadProject(argv[1]);
        QString fileName = argv[1];
        QString ext = QFileInfo(fileName).suffix();
        ext.toLower();
        if (ext == "pro") {
            app.loadProject(fileName);
        } else if (ext == "go") {
            app.loadEditor(fileName);
        }
    }

    return a.exec();
}
