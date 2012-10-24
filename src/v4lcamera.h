#ifndef V4LCAMERA_H
#define V4LCAMERA_H

#include <libv4l2.h>
#include <libv4lconvert.h>
#include <linux/videodev2.h>

#include "camerathread.h"
#include "configurationengine.h"
#include "settings.h"


class V4LCamera : public CameraThread
{
private:
    /// Configuration Engine
    ConfigurationEngine *_configEngine;

    /// Capture Device Name and File descriptor;
    QString _deviceName;
    int     _fd;

    /// V4L Support Structures
    enum   v4l2_buf_type        _type;
    struct v4l2_buffer          _buf;
    struct v4l2_format          _fmt, _src_fmt;
    struct v4l2_requestbuffers  _req;
    struct v4lconvert_data      *_v4lconvert_data;

    /// Internal Buffer for Memory Mapping
    Buffer          *_buffers;
    unsigned int    _n_buffers;

    /// select() support data
    int             _r;
    fd_set          _fds;
    struct timeval  _tv;

    /// Used to Open Capture Device and set V4l Conversion format
    void openCaptureDevice();

private:
    int exec();
    void run();

private slots:
    void configure();

public:
    V4LCamera();
    ~V4LCamera();

    int getHeight();
    int getWidth();
    bool isConfigurated();

public slots:
    void stop();
};

#endif // V4LCAMERA_H
