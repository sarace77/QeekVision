#ifndef GIGECAMERA_H
#define GIGECAMERA_H

#include "camerathread.h"
#include "gigesettings.h"

#if !defined WIN32 && !defined _WIN32 && !defined _LINUX
#define _LINUX
#endif

#if defined(_x64) || defined (__x86_64) || defined (_M_X64)
#define _x64 1
#elif defined(_x86) || defined(__i386) || defined (_M_IX86)
#define _x86 1
#endif

#include <PvApi.h>
#ifdef WIN32
#  include <io.h>
#else
#  include <unistd.h>
#endif

#define MAX_CAMERAS 10

class GigECamera : public CameraThread
{

private:
    typedef struct {
        unsigned long   UID;
        tPvHandle       Handle;
        tPvFrame        Frame;
        } tCamera;

    tPvCameraInfo   cameraList[MAX_CAMERAS];
    tPvCameraInfo   camInfo;
    tPvIpSettings   ipSettings;
    tPvErr          PvResult;

    tCamera Camera;

    int numCameras;

    GigESettings *settingsDialog;

    void printPvError();

private slots:
    void configure();
    int exec();
    void run();

    void viewCameraNetInfo();

public:
    GigECamera();
    ~GigECamera();

    int getHeight();
    int getWidth();
    bool isConfigurated();

public slots:
//    Mat getFrame();
    void stop();

};

#endif // GIGECAMERA_H
