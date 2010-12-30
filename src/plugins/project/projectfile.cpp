#include "projectfile.h"

#include <QFile>
#include <QDebug>
#include <QFileInfo>

ProjectFile::ProjectFile(QObject *parent) :
        QObject(parent)
{
}

ProjectFile::~ProjectFile()
{
    qDebug() << "~ProjectFile";
}

QString ProjectFile::displayName() const
{
    return QFileInfo(_filePath).completeBaseName();
}

QString ProjectFile::projectPath() const
{
    return QFileInfo(_filePath).absolutePath();
}

QString ProjectFile::fileName() const
{
    return QFileInfo(_filePath).fileName();
}

QString ProjectFile::filePath() const
{
    return _filePath;
}

void ProjectFile::close()
{
    context.clear();
}

bool ProjectFile::open(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    if (!file.isOpen())
        return false;

    QString all = file.readAll();
    all.replace(QRegExp("\\\\[\\s]*[\n\r]+[\\s]*")," ");
    QStringList list = all.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    context.clear();
    foreach(QString line, list) {
        QStringList v = line.split(QRegExp("\\+="),QString::SkipEmptyParts);
        if (v.count() == 1) {
            v = line.split(QRegExp("="),QString::SkipEmptyParts);
            if (v.count() == 2) {
                QStringList v2 = v.at(1).split(" ",QString::SkipEmptyParts);
                if (!v2.isEmpty()) {
                    context[v.at(0).trimmed()] = v2;
                }
            }
        } else if (v.count() == 2) {
            QStringList v2 = v.at(1).split(" ",QString::SkipEmptyParts);
            if (!v2.isEmpty())
                context[v.at(0).trimmed()].append(v2);
        }
    }
    file.close();
    _filePath = fileName;
    return !context.isEmpty();
}

QStringList ProjectFile::values(const QString &key)
{
    return context.value(key);
}

QStringList ProjectFile::goFiles()
{
    return values("GOFILES");
}

QStringList ProjectFile::headerFiles()
{
    return values("HEADERS");
}

QStringList ProjectFile::sourceFiles()
{
    return values("SOURCES");
}
