#-------------------------------------------------
#
# Project created by QtCreator 2016-07-22T17:36:15
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DefState
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    sleeper.h

FORMS    += mainwindow.ui

RC_FILE = res.rc

#DEFINES += QT_STATIC

#DISTFILES +=

RESOURCES += \
    mainwindow.qrc
