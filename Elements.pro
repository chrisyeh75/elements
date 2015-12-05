#-------------------------------------------------
#
# Project created by QtCreator 2015-10-22T13:21:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Elements
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    game.cpp

HEADERS  += mainwindow.h \
    game.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

CONFIG += c++11
