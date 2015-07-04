#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T02:59:46
#
#-------------------------------------------------

QT += core gui widgets network xml

TARGET = NTScout
TEMPLATE = app
VERSION = 7.4.1

QMAKE_CXXFLAGS = -std=c++11

RESOURCES += \
    resource.qrc

FORMS += \
    countrywidget.ui \
    gridwidget.ui \
    loginwidget.ui \
    mainwindow.ui \
    progresswidget.ui \
    searchdialog.ui \
    settingsdialog.ui \
    summarywidget.ui

HEADERS += \
    bbapi.h \
    country.h \
    countrywidget.h \
    gridwidget.h \
    loginwidget.h \
    mainwindow.h \
    network.h \
    player.h \
    progresswidget.h \
    searchdialog.h \
    searchvalues.h \
    settings.h \
    settingsdialog.h \
    summarywidget.h \
    util.h \
    worker.h

SOURCES += \
    bbapi.cpp \
    countrywidget.cpp \
    gridwidget.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    network.cpp \
    progresswidget.cpp \
    searchdialog.cpp \
    settings.cpp \
    settingsdialog.cpp \
    summarywidget.cpp \
    util.cpp \
    worker.cpp

