#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T02:59:46
#
#-------------------------------------------------

QT += core gui widgets network xml

TARGET = NTScout
TEMPLATE = app
VERSION = 7.3.0

QMAKE_CXXFLAGS = -std=c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    bbapi.cpp \
    countrywidget.cpp \
    loginwidget.cpp \
    network.cpp \
    util.cpp \
    gridwidget.cpp \
    searchdialog.cpp \
    progresswidget.cpp \
    worker.cpp \
    summarywidget.cpp \
    settingsdialog.cpp \
    settings.cpp

HEADERS  += mainwindow.h \
    bbapi.h \
    country.h \
    countrywidget.h \
    loginwidget.h \
    network.h \
    player.h \
    util.h \
    gridwidget.h \
    searchdialog.h \
    progresswidget.h \
    searchvalues.h \
    worker.h \
    summarywidget.h \
    settingsdialog.h \
    settings.h

FORMS    += mainwindow.ui \
    countrywidget.ui \
    loginwidget.ui \
    gridwidget.ui \
    searchdialog.ui \
    progresswidget.ui \
    summarywidget.ui \
    settingsdialog.ui

RESOURCES += \
    resource.qrc
