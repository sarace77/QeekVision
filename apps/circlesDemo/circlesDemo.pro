#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T14:53:33
#
#-------------------------------------------------

QT       += core gui

TARGET = circlesDemo
TEMPLATE = app


SOURCES += main.cpp\
        circlesmainwindow.cpp \
    displaywidget.cpp

HEADERS  += circlesmainwindow.h \
    displaywidget.h

FORMS    += circlesmainwindow.ui \
    displaywidget.ui

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
