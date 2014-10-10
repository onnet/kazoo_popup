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

win32 {
RC_FILE += rc.rc
}
