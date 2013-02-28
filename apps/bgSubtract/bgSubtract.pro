#-------------------------------------------------
#
# Project created by QtCreator 2012-10-20T16:40:14
#
#-------------------------------------------------

QT       += core gui

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

TARGET = bgSubtract
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
