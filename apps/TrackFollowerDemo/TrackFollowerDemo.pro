#-------------------------------------------------
#
# Project created by QtCreator 2012-11-30T22:57:35
#
#-------------------------------------------------

QT       += core gui

TARGET = TrackFollowerDemo
TEMPLATE = app


SOURCES += main.cpp\
        trackfollowerdemo.cpp

HEADERS  += trackfollowerdemo.h

FORMS    += trackfollowerdemo.ui

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
