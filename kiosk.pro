#-------------------------------------------------
#
# Project created by QtCreator 2013-11-03T07:44:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kiosk
TEMPLATE = app


SOURCES += main.cpp\
        kioskwindow.cpp \
    trimet.cpp

HEADERS  += kioskwindow.h \
    trimet.h

FORMS    += kioskwindow.ui

LIBS     += -lqjson

OTHER_FILES +=

RESOURCES += \
    resources.qrc
