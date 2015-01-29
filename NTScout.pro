#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T02:59:46
#
#-------------------------------------------------

QT += core gui widgets network xml

TARGET = NTScout
TEMPLATE = app
VERSION = 7.4.0

QMAKE_CXXFLAGS = -std=c++11

RESOURCES += \
    resource.qrc

FORMS += \
    source/countrywidget.ui \
    source/gridwidget.ui \
    source/loginwidget.ui \
    source/mainwindow.ui \
    source/progresswidget.ui \
    source/searchdialog.ui \
    source/settingsdialog.ui \
    source/summarywidget.ui

HEADERS += \
    source/bbapi.h \
    source/country.h \
    source/countrywidget.h \
    source/gridwidget.h \
    source/loginwidget.h \
    source/mainwindow.h \
    source/network.h \
    source/player.h \
    source/progresswidget.h \
    source/searchdialog.h \
    source/searchvalues.h \
    source/settings.h \
    source/settingsdialog.h \
    source/summarywidget.h \
    source/util.h \
    source/worker.h

SOURCES += \
    source/bbapi.cpp \
    source/countrywidget.cpp \
    source/gridwidget.cpp \
    source/loginwidget.cpp \
    source/main.cpp \
    source/mainwindow.cpp \
    source/network.cpp \
    source/progresswidget.cpp \
    source/searchdialog.cpp \
    source/settings.cpp \
    source/settingsdialog.cpp \
    source/summarywidget.cpp \
    source/util.cpp \
    source/worker.cpp

