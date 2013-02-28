TEMPLATE = lib

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
LIBS += -lv4lconvert

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
    mpgwriter.h \
    circles.h \
    opencvcamera.h \
    ellipseobject.h \
    qvdisplaywidget.h \
    qvdriverselect.h \
    histograms.h

SOURCES += \
    camerathread.cpp \
    thermography.cpp \
    v4lcamera.cpp \
    v4lconfigurationengine.cpp \
    v4lsettings.cpp \
    processthread.cpp \
    bgsubtractor.cpp \
    mpgwriter.cpp \
    circles.cpp \
    opencvcamera.cpp \
    ellipseobject.cpp \
    qvdisplaywidget.cpp \
    qvdriverselect.cpp \
    histograms.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    v4lsettings.ui \
    gigesettings.ui \
    qvdisplaywidget.ui \
    qvdriverselect.ui

CONFIG += debug_and_release

CONFIG(debug, debug|release) {
    DEFINES += _DEBUG_CAPTURE_THREADS \
                _DEBUG_PROCESS_THREADS
}

