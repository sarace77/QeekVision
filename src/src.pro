TEMPLATE = lib

LIBS += -L/opt/AVT_GigE_SDK/lib-pc/x64/4.5
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

exists(/opt/AVT_GigE_SDK) {
    DEFINES += _ENABLE_GIG_E_CAMERA_SUPPORT
    INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/
    LIBS += -lPvAPI
    HEADERS +=  gigecamera.h \
                gigesettings.h
    SOURCES +=  gigecamera.cpp \
                gigesettings.cpp
} else {
    warning("No AVT_GigE SDK Found!")
    warning("Please symlink AVT GigE SDK main folder to /opt/AVT_GigE_SDK in order to enable its support!")
}
