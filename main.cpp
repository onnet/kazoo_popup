#include "mainwindow.h"
#include "logger.h"
#include "updatemanager.h"

#include <QtSingleApplication>

int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);

    if (a.isRunning())
        return 0;

    a.setQuitOnLastWindowClosed(false);
    a.setApplicationVersion(APP_VERSION);

    Logger::instance()->start();

    UpdateManager::instance()->start();

    MainWindow w;
    w.hide();

    return a.exec();
}
