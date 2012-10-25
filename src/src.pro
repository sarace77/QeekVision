TEMPLATE = lib

LIBS += -L/usr/local/lib/
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
LIBS += -lv4lconvert
INCLUDEPATH += /usr/local/include/opencv-2.4.2
DEPENDPATH += /usr/local/include/opencv-2.4.2

TARGET = QeekVision
VERSION = 0.1.0

HEADERS += \
    Defs.h \
    camerathread.h \
    thermography.h \
    v4lcamera.h \
    v4lconfigurationengine.h \
    v4lsettings.h \
    processthread.h \
    bgsubtractor.h \
    mpgwriter.h

SOURCES += \
    camerathread.cpp \
    thermography.cpp \
    v4lcamera.cpp \
    v4lconfigurationengine.cpp \
    v4lsettings.cpp \
    processthread.cpp \
    bgsubtractor.cpp \
    mpgwriter.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    v4lsettings.ui

