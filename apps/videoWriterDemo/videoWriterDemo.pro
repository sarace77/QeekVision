#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T17:10:25
#
#-------------------------------------------------

QT       += core gui

TARGET = videoWriterDemo
TEMPLATE = app


SOURCES += main.cpp\
        videowriterdemowindow.cpp

HEADERS  += videowriterdemowindow.h

FORMS    += videowriterdemowindow.ui

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

