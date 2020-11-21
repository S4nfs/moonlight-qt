#include "path.h"

#include <QtDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>
#include <QCoreApplication>

QString Path::s_CacheDir;
QString Path::s_LogDir;
QString Path::s_BoxArtCacheDir;

QString Path::getLogDir()
{
    Q_ASSERT(!s_LogDir.isEmpty());
    return s_LogDir;
}

QString Path::getBoxArtCacheDir()
{
    Q_ASSERT(!s_BoxArtCacheDir.isEmpty());
    return s_BoxArtCacheDir;
}

QByteArray Path::readDataFile(QString fileName)
{
    QFile dataFile(getDataFilePath(fileName));
    dataFile.open(QIODevice::ReadOnly);
    return dataFile.readAll();
}

void Path::writeDataFile(QString fileName, QByteArray data)
{
    QFile dataFile(QDir(s_CacheDir).absoluteFilePath(fileName));
    dataFile.open(QIODevice::WriteOnly);
    dataFile.write(data);
}

QString Path::getDataFilePath(QString fileName)
{
    QString candidatePath;

    // Check the cache location first (used by Path::writeDataFile())
    candidatePath = QDir(s_CacheDir).absoluteFilePath(fileName);
    if (QFile::exists(candidatePath)) {
        qInfo() << "Found" << fileName << "at" << candidatePath;
        return candidatePath;
    }

    // Check the current directory
    candidatePath = QDir(QDir::currentPath()).absoluteFilePath(fileName);
    if (QFile::exists(candidatePath)) {
        qInfo() << "Found" << fileName << "at" << candidatePath;
        return candidatePath;
    }

    // Now check the data directories (for Linux, in particular)
    candidatePath = QStandardPaths::locate(QStandardPaths::AppDataLocation, fileName);
    if (!candidatePath.isEmpty() && QFile::exists(candidatePath)) {
        qInfo() << "Found" << fileName << "at" << candidatePath;
        return candidatePath;
    }

    // Now try the directory of our app installation (for Windows, if current dir doesn't find it)
    candidatePath = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(fileName);
    if (QFile::exists(candidatePath)) {
        qInfo() << "Found" << fileName << "at" << candidatePath;
        return candidatePath;
    }

    // Return the QRC embedded copy
    candidatePath = ":/data/" + fileName;
    qInfo() << "Found" << fileName << "at" << candidatePath;
    return QString(candidatePath);
}

void Path::initialize(bool portable)
{
    if (portable) {
        s_LogDir = QDir::currentPath();
        s_BoxArtCacheDir = QDir::currentPath() + "/boxart";
        s_CacheDir = QDir::currentPath();
    }
    else {
#ifdef Q_OS_DARWIN
        // On macOS, $TMPDIR is some random folder under /var/folders/ that nobody can
        // easily find, so use the system's global tmp directory instead.
        s_LogDir = "/tmp";
#else
        s_LogDir = QDir::tempPath();
#endif
        s_CacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        s_BoxArtCacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/boxart";
    }
}
