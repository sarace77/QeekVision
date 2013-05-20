QT       += core gui opengl

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

LIBS += -L$$OUT_PWD/../../src/ -lQeekVision
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

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
