#-------------------------------------------------
#
# Project created by QtCreator 2012-10-24T17:10:25
#
#-------------------------------------------------

QT       += core gui

LIBS += -L/usr/local/lib/
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/
INCLUDEPATH += $$PWD/../../src

DEPENDPATH += $$PWD/../../src
DEPENDPATH += /usr/local/include/

TARGET = videoWriterDemo
TEMPLATE = app

SOURCES += main.cpp\
        videowriterdemowindow.cpp

HEADERS  += videowriterdemowindow.h

FORMS    += videowriterdemowindow.ui

exists(/opt/AVT_GigE_SDK) {
    DEFINES += _ENABLE_GIG_E_CAMERA_SUPPORT
}
