#-------------------------------------------------
#
# Project created by QtCreator 2014-10-31T11:39:28
#
#-------------------------------------------------

QT += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG -= app_bundle

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
    #ICON = res/mac/kazoo.icns
    #QMAKE_INFO_PLIST = res/mac/MyAppInfo.plist

    #OTHER_FILES += res/mac/MyAppInfo.plist

    #deploy.depends  += all
    #deploy.commands += macdeployqt $${TARGET}.app;

    #deploy.commands += cp $${PWD}/3rdparty/quazip/lib/libquazip.1.dylib $${TARGET}.app/Contents/Frameworks/libquazip.1.dylib;

    # Remove unneeded plugins
    #deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/printsupport;

    #QMAKE_EXTRA_TARGETS += deploy

    deploy.commands += install_name_tool -change libquazip.1.dylib @executable_path/../Frameworks/libquazip.1.dylib autoupdater;
    deploy.commands += install_name_tool -change /Users/admin/Qt5.3.0/5.3/clang_64/lib/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore autoupdater;
    deploy.commands += install_name_tool -change /Users/admin/Qt5.3.0/5.3/clang_64/lib/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui autoupdater;
    deploy.commands += install_name_tool -change /Users/admin/Qt5.3.0/5.3/clang_64/lib/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets autoupdater;
    deploy.commands += install_name_tool -change /Users/admin/Qt5.3.0/5.3/clang_64/lib/QtNetwork.framework/Versions/5/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/5/QtNetwork autoupdater;

    QMAKE_EXTRA_TARGETS += deploy
}
