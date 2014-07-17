#-------------------------------------------------
#
# Project created by QtCreator 2014-07-15T16:29:41
#
#-------------------------------------------------

QT       += core gui websockets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebSocket
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    websocketmanager.cpp \
    informerdialog.cpp

HEADERS  += mainwindow.h \
    websocketmanager.h \
    defaults.h \
    informerdialog.h \
    contactinfo.h

FORMS    += mainwindow.ui \
    informerdialog.ui

RESOURCES += \
    app.qrc
