#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T16:29:41
#
#-------------------------------------------------

QT       += core gui websockets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(qtsingleapplication/qtsingleapplication.pri)

TARGET = KazooPopup
TEMPLATE = app

CONFIG += c++11

# Application version
VERSION_MAJOR=1
VERSION_MINOR=0
VERSION_PATCH=0
DEFINES += APP_VERSION=\\\"$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}\\\"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    websocketmanager.cpp \
    informerdialog.cpp \
    caller.cpp \
    defaults.cpp \
    debugdialog.cpp \
    logger.cpp

HEADERS  += \
    mainwindow.h \
    websocketmanager.h \
    defaults.h \
    informerdialog.h \
    caller.h \
    debugdialog.h \
    logger.h

win32 | macx {
    SOURCES += updatemanager.cpp
    HEADERS += updatemanager.h
}

FORMS    += \
    mainwindow.ui \
    informerdialog.ui \
    debugdialog.ui

RESOURCES += \
    app.qrc

macx {
    RESOURCES += macx.qrc
}

win32 {
    RC_FILE += rc.rc
}

macx {
    ICON = res/mac/kazoo.icns
    QMAKE_INFO_PLIST = res/mac/MyAppInfo.plist

    OTHER_FILES += res/mac/MyAppInfo.plist

    deploy.depends  += all
    deploy.commands += macdeployqt $${TARGET}.app;

    deploy.commands += cp $${PWD}/setup/settings.ini $${TARGET}.app/Contents/MacOS/settings.ini;

    # Remove unneeded plugins
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/printsupport;
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/sqldrivers;

    # Build the product package
    #product.depends += all
    #product.commands += pkgbuild --identifier 2600hz --version 1.0 --root KazooPopup.app --install-location ~/Applications/KazooPopup.app KazooPopup.pkg;

    makedmg.commands += sh $${PWD}/res/mac/make_dmg.sh -V -i $${PWD}/$${ICON} -s "400:300" -c "300:50:100:50" "$${TARGET}.app";

    QMAKE_EXTRA_TARGETS += deploy makedmg
}
