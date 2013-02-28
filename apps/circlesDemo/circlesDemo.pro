#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T14:53:33
#
#-------------------------------------------------

QT       += core gui

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

TARGET = circlesDemo
TEMPLATE = app

SOURCES += main.cpp\
        circlesmainwindow.cpp

HEADERS  += circlesmainwindow.h

FORMS    += circlesmainwindow.ui
