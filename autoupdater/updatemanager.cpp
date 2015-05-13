#include "updatemanager.h"

#include <QFile>
#include <QDir>

#include <QDebug>

bool UpdateManager::makeBackup(const QString &sourceFolder,
                             const QString &backupFolderName,
                             const QStringList &ignoreList)
{
    QDir targetDir(sourceFolder);
    if (!targetDir.mkdir(backupFolderName))
    {
        qWarning("Unable to create backup dir");
        return false;
    }

    return copyFiles(sourceFolder, sourceFolder + "/" + backupFolderName, ignoreList);
}

bool UpdateManager::installUpdates(const QString &updatesDirPath,
                                 const QString &targetDirPath,
                                 const QStringList &ignoreList)
{
    return copyFiles(updatesDirPath, targetDirPath, ignoreList);
}

bool UpdateManager::restoreFromBackup(const QString &targetDirPath, const QString &backupFolderName)
{
    return moveFiles(backupFolderName, targetDirPath);
}

bool UpdateManager::copyFiles(const QString &sourceDirPath,
                            const QString &destDirPath,
                            const QStringList &ignoreList)
{
    QDir sourceDir(sourceDirPath);
    QDir destDir(destDirPath);

    QStringList sourceFiles = sourceDir.entryList(QDir::Files);
    foreach (const QString &entry, sourceFiles)
    {
        QString source = sourceDir.absolutePath() + "/" + entry;
        QString dest = destDir.absolutePath() + "/" + entry;

        if (ignoreList.contains(entry))
            continue;

        if (QFile::exists(dest))
            if (!QFile::remove(dest))
                return false;

        if (!QFile::copy(source, dest))
            return false;
    }

    QStringList sourceSubDirs = sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (const QString &entry, sourceSubDirs)
    {
        if (ignoreList.contains(entry))
            continue;

        QString sourceFolder = sourceDir.absolutePath() + "/" + entry;
        QString destFolder = destDir.absolutePath() + "/" + entry;

        QDir destDir(destFolder);
        if (destDir.exists())
            if (!destDir.removeRecursively())
                return false;

        if (!QDir().mkpath(destFolder))
            return false;

        if (!copyFiles(sourceFolder, destFolder))
            return false;
    }
    return true;
}

bool UpdateManager::moveFiles(const QString &sourceDirPath,
                            const QString &destDirPath,
                            const QStringList &ignoreList)
{
    QDir sourceDir(sourceDirPath);
    QDir destDir(destDirPath);

    QStringList sourceFiles = sourceDir.entryList(QDir::Files);
    foreach (const QString &entry, sourceFiles)
    {
        if (ignoreList.contains(entry))
            continue;

        QString source = sourceDir.absolutePath() + "/" + entry;
        QString dest = destDir.absolutePath() + "/" + entry;

        if (QFile::exists(dest))
            if (!QFile::remove(dest))
                return false;

        if (!QFile::rename(source, dest))
            return false;
    }

    QStringList sourceSubDirs = sourceDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (const QString &entry, sourceSubDirs)
    {
        if (ignoreList.contains(entry))
            continue;

        QString sourceFolder = sourceDir.absolutePath() + "/" + entry;
        QString destFolder = destDir.absolutePath() + "/" + entry;

        if (!QDir().rename(sourceFolder, destFolder))
            return false;
    }
    return true;
}
