#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "camerathread.h"

#include <QCheckBox>
#include <QTimer>

using namespace cv;

class OpenCVCamera : public CameraThread
{
    Q_OBJECT

private:
    /// Capture Device Name and File descriptor;
    QString _deviceName;

    /// Video Capture device
    VideoCapture *_camera;

    /// Watchdog
    QTimer _watchdog;

    /// Enable Verbose
    QCheckBox *_enableVerboseMode;

private:
    int exec();
    void run();

private slots:
    void configure();
    void timeout();

public:
    OpenCVCamera();
    ~OpenCVCamera();

    int getHeight();
    int getWidth();
    bool isConfigurated();

public slots:
    void stop();

};

#endif // OPENCVCAMERA_H
