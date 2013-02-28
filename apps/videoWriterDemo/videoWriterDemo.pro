#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T17:10:25
#
#-------------------------------------------------

QT       += core gui

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

TARGET = videoWriterDemo
TEMPLATE = app

SOURCES += main.cpp\
        videowriterdemowindow.cpp

HEADERS  += videowriterdemowindow.h

FORMS    += videowriterdemowindow.ui
