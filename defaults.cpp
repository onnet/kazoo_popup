#include "defaults.h"

#include <QString>
#include <QStandardPaths>
#include <QApplication>
#include <QDir>

#ifdef Q_OS_MAC
#   include <QProcess>
#endif

static const char * const kLogsDirName = "logs";

QString dataDirPath()
{
    QString appDirPath("%1/%2");
    appDirPath = appDirPath.arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation),
                                qApp->applicationName());
    QDir appDir(appDirPath);
    if (!appDir.exists())
        appDir.mkpath(appDirPath);

#ifdef Q_OS_MAC
    if (!appDir.exists("settings.ini"))
        QProcess::startDetached("sh", QStringList() << "-c" << "cp '" + qApp->applicationDirPath() + "/settings.ini' '" + appDirPath + "/settings.ini'");
#endif

    return appDirPath;
}

QString logsDirPath()
{
    QString path = dataDirPath();
    QDir dataDir(path);
    if (!dataDir.exists(kLogsDirName))
        dataDir.mkdir(kLogsDirName);
    return QString("%1/%2/").arg(path, kLogsDirName);
}

QString detectPlatform()
{
#ifdef Q_OS_WIN
    return "win";
#elif defined Q_OS_MAC
    return "mac";
#endif
}
