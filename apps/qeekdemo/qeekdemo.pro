QT       += core gui

LIBS += -L/usr/local/lib/
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/
INCLUDEPATH += $$PWD/../../src

DEPENDPATH += $$PWD/../../src
DEPENDPATH += /usr/local/include/

TEMPLATE = app
TARGET = qeekdemo
VERSION = 1.0.0


HEADERS += \
    qeekdemowindow.h

SOURCES += \
    main.cpp \
    qeekdemowindow.cpp

FORMS += \
    qeekdemowindow.ui

