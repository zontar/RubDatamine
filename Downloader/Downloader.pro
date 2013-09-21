#-------------------------------------------------
#
# Project created by QtCreator 2013-09-21T19:51:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Downloader
TEMPLATE = app


SOURCES += main.cpp\
        downloader.cpp \
    downloadmanager.cpp

HEADERS  += downloader.h \
    downloadmanager.h

FORMS    += downloader.ui
