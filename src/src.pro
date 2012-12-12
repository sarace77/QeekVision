TEMPLATE = lib

LIBS += -L/usr/local/lib/
LIBS += -L/opt/AVT_GigE_SDK/lib-pc/x64/4.5
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video
LIBS += -lv4lconvert
LIBS += -lPvAPI

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /opt/AVT_GigE_SDK/inc-pc/

DEPENDPATH += /usr/local/include/

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
    gigecamera.h \
    gigesettings.h \
    qvdisplaywidget.h \
    qvdriverselect.h

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
    gigecamera.cpp \
    gigesettings.cpp \
    qvdisplaywidget.cpp \
    qvdriverselect.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    v4lsettings.ui \
    gigesettings.ui \
    qvdisplaywidget.ui \
    qvdriverselect.ui

