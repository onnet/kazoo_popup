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

#unix:!macx:!symbian {
#    LIBS += -L/usr/local/lib/ -lquazip
#    INCLUDEPATH += /usr/local/include/quazip
#    DEPENDPATH += /usr/local/include/quazip
#}

win32 {
    LIBS += -L$$PWD/3rdparty/quazip/lib -lquazip
    INCLUDEPATH += $$PWD/3rdparty/quazip/include

    RC_FILE += rc.rc
}
