#include "defaults.h"

#include <QString>
#include <QStandardPaths>
#include <QApplication>
#include <QDir>

QString dataDirPath()
{
    QString appDirPath("%1/%2");
    appDirPath = appDirPath.arg(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation),
                                qApp->applicationName());
    QDir appDir(appDirPath);
    if (!appDir.exists())
        appDir.mkpath(appDirPath);

    return appDirPath;
}
