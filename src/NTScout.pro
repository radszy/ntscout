#-------------------------------------------------
#
# Project created by QtCreator 2014-10-09T02:59:46
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = NTScout
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bbapi.cpp \
    countrygridwidget.cpp \
    countrywidget.cpp \
    loginwidget.cpp \
    network.cpp

HEADERS  += mainwindow.h \
    bbapi.h \
    countrygridwidget.h \
    countrywidget.h \
    loginwidget.h \
    network.h \
    player.h

FORMS    += mainwindow.ui \
    countrygridwidget.ui \
    countrywidget.ui \
    loginwidget.ui
