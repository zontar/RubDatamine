#-------------------------------------------------
#
# Project created by QtCreator 2013-09-22T15:04:40
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BDMaker
TEMPLATE = app


SOURCES += main.cpp\
        bdmaker.cpp \
    postparser.cpp \
    userparser.cpp \
    sectionparser.cpp \
    threadparser.cpp \
    abstractparser.cpp

HEADERS  += bdmaker.h \
    postparser.h \
    userparser.h \
    ../common/common.h \
    sectionparser.h \
    threadparser.h \
    abstractparser.h

FORMS    += bdmaker.ui

INCLUDEPATH += ../common
