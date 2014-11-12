#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QStringList>

class QString;

class UpdateManager
{
public:
    static bool makeBackup(const QString &sourceFolder,
                           const QString &backupFolderName,
                           const QStringList &ignoreList);

    static bool installUpdates(const QString &updatesDirPath,
                               const QString &targetDirPath,
                               const QStringList &ignoreList);

    static bool restoreFromBackup(const QString &targetDirPath,
                                  const QString &backupFolderName);

private:
    static bool moveFiles(const QString &sourceDirPath,
                          const QString &destDirPath,
                          const QStringList &ignoreList = QStringList());
    static bool copyFiles(const QString &sourceDirPath,
                          const QString &destDirPath,
                          const QStringList &ignoreList = QStringList());
};

#endif // UPDATEMANAGER_H
