#-------------------------------------------------
#
# Project created by QtCreator 2012-11-30T22:57:35
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

TARGET = TrackFollowerDemo
TEMPLATE = app

SOURCES += main.cpp\
        trackfollowerdemo.cpp

HEADERS  += trackfollowerdemo.h

FORMS    += trackfollowerdemo.ui

exists(/opt/AVT_GigE_SDK) {
    DEFINES += _ENABLE_GIG_E_CAMERA_SUPPORT
}
