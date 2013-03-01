#ifndef OPENCVCAMERA_H
#define OPENCVCAMERA_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Defs.h"
#include "camerathread.h"

using namespace cv;

class OpenCVCamera : public CameraThread
{
    Q_OBJECT
public:
    OpenCVCamera();
    ~OpenCVCamera();

    int getHeight();
    int getWidth();
    bool isConfigurated();

public slots:
    void stop();

private:
    /// Capture Device Name and File descriptor;
    QString _deviceName;

    /// Video Capture device
    VideoCapture *_camera;

    /// Enable Verbose
    QCheckBox *_enableVerboseMode;

    /// Frame Size
    int _width_, _height_;

private slots:
    void configure();
    int exec();
    void run();
};

#endif // OPENCVCAMERA_H
