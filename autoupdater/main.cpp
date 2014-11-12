#include "autoupdater.h"

#include <QtSingleApplication>
#include <QStringList>

int main(int argc, char *argv[])
{
    QtSingleApplication app(argc, argv);

    if (app.isRunning())
        return 0;

    app.setQuitOnLastWindowClosed(false);

    QStringList arguments(app.arguments());
    if (arguments.count() < 2 || arguments.count() > 3)
        return 0;

    QString action = arguments.at(1);

    // Select action
    AutoUpdater autoUpdater;
    if (action == "update" && arguments.count() == 3)
        autoUpdater.downloadUpdates(arguments.at(2));
    else if (action == "install")
        autoUpdater.installNewVersion();
    else if (action == "clean")
        autoUpdater.clean();
    else
        return 0;

    return app.exec();
}
