#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "../../api/iproject.h"

class ProjectFile : public QObject , public IProject
{
public:
    QStringList sourceFiles();
    QStringList headerFiles();
    QStringList goFiles();
    virtual void close();
    QString projectPath() const;
    ProjectFile(QObject *parent = 0);
    virtual QString displayName() const;
    virtual QString filePath() const;
    virtual QString fileName() const;
    virtual bool open(const QString &fileName);
    virtual QStringList values(const QString &key);
public:
    QMap<QString,QStringList>   context;
    QString _filePath;
};

#endif // PROJECTFILE_H
