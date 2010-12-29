#ifndef PROJECTFILE_H
#define PROJECTFILE_H

#include <QObject>
#include <QMap>
#include <QStringList>

class ProjectFile : public QObject
{
public:
    QStringList sourceFiles();
    QStringList headerFiles();
    QStringList goFiles();
    void close();
    ProjectFile(QObject *parent = 0);
    QString projectName() const;
    QString projectPath() const;
    QString filePath() const;
    QString fileName() const;
public:
    bool openProject(const QString &fileName);
    QStringList values(const QString &key);
public:
    QMap<QString,QStringList>   context;
    QString _filePath;
};

#endif // PROJECTFILE_H
