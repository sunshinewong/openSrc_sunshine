#-------------------------------------------------
#
# Project created by QtCreator 2014-01-21T16:25:59
#
#-------------------------------------------------

QT  += core gui  network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
LIBS     += -lqjson
TARGET = sunshine
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    http.cpp \
    loadwaitgif.cpp

HEADERS  += widget.h \
    http.h \
    loadwaitgif.h

FORMS    += widget.ui

RESOURCES += \
    weather.qrc
