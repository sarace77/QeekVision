#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T14:53:33
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

TARGET = circlesDemo
TEMPLATE = app

SOURCES += main.cpp\
        circlesmainwindow.cpp

HEADERS  += circlesmainwindow.h

FORMS    += circlesmainwindow.ui


exists(/opt/AVT_GigE_SDK) {
    DEFINES += _ENABLE_GIG_E_CAMERA_SUPPORT
}
