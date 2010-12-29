#include "projectplugin.h"
#include "projectwizard.h"

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>

ProjectPlugin::ProjectPlugin()
{
}

void ProjectPlugin::createActions()
{
    newProjectAct = new QAction(tr("New Project"),this);
    newProjectAct->setToolTip(tr("Create New Project"));
    connect(newProjectAct,SIGNAL(triggered()),this,SLOT(newProject()));

    openProjectAct = new QAction(tr("Open Project"),this);
    openProjectAct->setToolTip(tr("Open Project"));
    connect(openProjectAct,SIGNAL(triggered()),this,SLOT(openProject()));

    closeProjectAct = new QAction(tr("Close Project"),this);
    closeProjectAct->setToolTip(tr("Close Project"));
    connect(closeProjectAct,SIGNAL(triggered()),this,SLOT(closeProject()));

    QMenu *fileMenu = liteApp->fileMenu();
    QAction *seperator = fileMenu->insertSeparator(fileMenu->actions()[0]);
    fileMenu->insertActions(seperator,QList<QAction*>() << newProjectAct
                            << openProjectAct << closeProjectAct);
}

void ProjectPlugin::install(IApplication *app)
{
    if (!app)
        return;
    liteApp = app;
    createActions();
}

void ProjectPlugin::uninstall()
{
}

QString ProjectPlugin::name() const
{
    return tr("Project");
}

QString ProjectPlugin::info() const
{
    return tr("Project Plugin");
}

Q_EXPORT_PLUGIN(ProjectPlugin)



void ProjectPlugin::newProject()
{
    ProjectWizard wiz;
    if (wiz.exec() == QDialog::Accepted) {
        IProject * proj = createProject(wiz.projectLocation,wiz.projectName);
    }
}

void ProjectPlugin::openProject()
{
}
void ProjectPlugin::closeProject()
{
}

IProject *ProjectPlugin::createProject(const QString &path, const QString &name)
{
    ProjectFile *proj = new ProjectFile;
    proj->_fullPath = QFileInfo(QDir(path),name+".litepro").filePath();

    QFile file(proj->fullPath());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!file.isOpen()) {
        delete proj;
        return NULL;
    }
    QTextStream stream(&file);
    stream << QString("TARGET=%1").arg(name);

    file.close();

    return proj;
}
