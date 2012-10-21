#-------------------------------------------------
#
# Project created by QtCreator 2012-10-20T16:40:14
#
#-------------------------------------------------

QT       += core gui

TARGET = bgSubtract
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv-2.4.2
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_video \
        -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
