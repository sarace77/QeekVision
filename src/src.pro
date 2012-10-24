TEMPLATE = lib

LIBS += -L/usr/local/lib/ -lopencv_core -lopencv_highgui -lopencv_imgproc -lv4lconvert
INCLUDEPATH += /usr/local/include/opencv-2.4.2
DEPENDPATH += /usr/local/include/opencv-2.4.2

TARGET = QeekVision
VERSION = 0.1.0

HEADERS += \
    settings.h \
    Defs.h \
    configurationengine.h \
    camerathread.h \
    thermography.h \
    v4lcamera.h

SOURCES += \
    settings.cpp \
    configurationengine.cpp \
    camerathread.cpp \
    thermography.cpp \
    v4lcamera.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    settings.ui

