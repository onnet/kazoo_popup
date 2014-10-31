#include "defaults.h"

#include <QString>
#include <QStandardPaths>
#include <QApplication>
#include <QDir>

static const char * const kLogsDirName = "logs";

QString dataDirPath()
{
#ifdef Q_OS_WIN
    QString appDirPath("%1/%2");
    appDirPath = appDirPath.arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation),
                                qApp->applicationName());
    QDir appDir(appDirPath);
    if (!appDir.exists())
        appDir.mkpath(appDirPath);

    return appDirPath;
#elif defined Q_OS_MAC
    return qApp->applicationDirPath();
#endif
}

QString logsDirPath()
{
    QString path = dataDirPath();
    QDir dataDir(path);
    if (!dataDir.exists(kLogsDirName))
        dataDir.mkdir(kLogsDirName);
    return QString("%1/%2/").arg(path, kLogsDirName);
}
