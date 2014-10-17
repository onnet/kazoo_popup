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

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    websocketmanager.cpp \
    informerdialog.cpp \
    caller.cpp \
    defaults.cpp

HEADERS  += \
    mainwindow.h \
    websocketmanager.h \
    defaults.h \
    informerdialog.h \
    caller.h

FORMS    += \
    mainwindow.ui \
    informerdialog.ui

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

    # Remove unneeded frameworks
    deploy.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtPrintSupport.framework;
    #deploy.commands += rm -r $${TARGET}.app/Contents/Frameworks/QtSql.framework;

    # Remove unneeded plugins
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/printsupport;
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/sqldrivers;

    # Build the product package
    #product.depends += all
    #product.commands += pkgbuild --identifier 2600hz --version 1.0 --root KazooPopup.app --install-location ~/Applications/KazooPopup.app KazooPopup.pkg;

    deploy.commands += sh $${PWD}/res/mac/make_dmg.sh -V -i $${PWD}/$${ICON} -s "400:300" "$${TARGET}.app";

    QMAKE_EXTRA_TARGETS += deploy
}
