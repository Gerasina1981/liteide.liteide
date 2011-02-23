#include "projectfile.h"

#include <QFile>
#include <QDebug>
#include <QFileInfo>

ProjectFile::ProjectFile(QObject *parent) :
        QObject(parent)
{
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

bool ProjectFile::reload()
{
    bool b = open(_filePath);
    emit reloadProject();
    return b;
}

void ProjectFile::close()
{
    emit closeProject();
    context.clear();
}

bool ProjectFile::open(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    if (!file.isOpen())
        return false;
    QStringList list;
    QString line;
    bool bnext = false;
    while (!file.atEnd()) {
        QByteArray ar = file.readLine().trimmed();
        if (!ar.isEmpty() && ar.right(1) == "\\") {
            bnext = true;
            ar[ar.length()-1] = ' ';
        } else {
            bnext = false;
        }
        line.push_back(ar);
        if (!bnext && !line.isEmpty()) {
            list.push_back(line);
            line.clear();
        }
    }
    file.close();

    if (!line.isEmpty()) {
        list.push_back(line);
    }
    _filePath = fileName;
    _isMakefile = QFileInfo(fileName).suffix().toLower() != "pro";

    context.clear();
    foreach (QString line, list) {
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
    return !context.isEmpty();
    /*
    QString all = file.readAll();
    all.replace(QRegExp("\\\\[\\s]*[\n\r]+[\\s]*")," ");
    QStringList list = all.split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    context.clear();
    foreach(QString line, list) {
         line.replace("\t"," ");
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
    */
}

QStringList ProjectFile::values(const QString &key)
{
    return context.value(key);
}

bool ProjectFile::isMakefile()
{
    return _isMakefile;
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
