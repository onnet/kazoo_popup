#-------------------------------------------------
#
# Project created by QtCreator 2014-10-31T11:39:28
#
#-------------------------------------------------

QT += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = autoupdater

include(qtsingleapplication/qtsingleapplication.pri)

TEMPLATE = app

SOURCES += \
    main.cpp \
    downloadmanager.cpp \
    autoupdater.cpp \
    zipextractor.cpp \
    updatemanager.cpp

HEADERS += \
    downloadmanager.h \
    autoupdater.h \
    zipextractor.h \
    defaults.h \
    updatemanager.h

RESOURCES += \
    res.qrc

LIBS += -L$$PWD/3rdparty/quazip/lib -lquazip
INCLUDEPATH += $$PWD/3rdparty/quazip/include

win32 {
    RC_FILE += rc.rc
}

macx {
    ICON = res/mac/kazoo.icns
    QMAKE_INFO_PLIST = res/mac/MyAppInfo.plist

    OTHER_FILES += res/mac/MyAppInfo.plist

    deploy.depends  += all
    deploy.commands += macdeployqt $${TARGET}.app;

    deploy.commands += cp $${PWD}/3rdparty/quazip/lib/libquazip.1.dylib $${TARGET}.app/Contents/Frameworks/libquazip.1.dylib;

    # Remove unneeded plugins
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/printsupport;

    QMAKE_EXTRA_TARGETS += deploy
}
