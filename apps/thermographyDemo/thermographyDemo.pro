#-------------------------------------------------
#
# Project created by QtCreator 2012-10-23T08:45:22
#
#-------------------------------------------------

QT       += core gui

TARGET = thermographyDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv-2.4.2
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

