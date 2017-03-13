#-------------------------------------------------
#
# Project created by QtCreator 2017-03-06T09:25:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test_obs
TEMPLATE = app

DESTDIR  = ./debug/bin/32bit

include(./obs/obs.pri)
#INCLUDEPATH += ./UI
#INCLUDEPATH += ./UI/obs-frontend-api

FORMS += ctestwidget.ui

HEADERS += ctestwidget.h \
    cobsbasic.h \
    cbasicoutputhandler.h \
    cobsglobal.h

SOURCES += ctestwidget.cpp \
  main.cpp \
    cobsbasic.cpp \
    cbasicoutputhandler.cpp \
    cobsglobal.cpp

