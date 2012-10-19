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



INCLUDEPATH += /usr/local/include/opencv-2.4.2
INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
LIBS += -L$$OUT_PWD/../../src/ -lQeekVision

