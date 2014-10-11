#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T02:59:46
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = NTScout
TEMPLATE = app

QMAKE_CXXFLAGS = -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    bbapi.cpp \
    countrywidget.cpp \
    loginwidget.cpp \
    network.cpp \
    util.cpp \
    gridwidget.cpp \
    searchdialog.cpp

HEADERS  += mainwindow.h \
    bbapi.h \
    country.h \
    countrywidget.h \
    loginwidget.h \
    network.h \
    player.h \
    util.h \
    gridwidget.h \
    searchdialog.h

FORMS    += mainwindow.ui \
    countrywidget.ui \
    loginwidget.ui \
    gridwidget.ui \
    searchdialog.ui

RESOURCES += \
    resource.qrc
