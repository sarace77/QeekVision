#-------------------------------------------------
#
# Project created by QtCreator 2012-12-07T10:58:42
#
#-------------------------------------------------

QT       += core gui

LIBS += -L/usr/local/lib/
#LIBS += -L/opt/AVT_GigE_SDK/bin-pc/x64/
#LIBS += -L/opt/AVT_GigE_SDK/lib-pc/x64/4.5/
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
#LIBS += -lPvAPI
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/
INCLUDEPATH += $$PWD/../../src

#DEPENDPATH += $$PWD/../../src
#DEPENDPATH += /usr/local/include/



TARGET = GigeQeekDemo
TEMPLATE = app


SOURCES += main.cpp\
        gigeqeekdemowindow.cpp

HEADERS  += gigeqeekdemowindow.h

FORMS    += gigeqeekdemowindow.ui
