#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <libv4l2.h>
#include <libv4lconvert.h>
#include <linux/videodev2.h>
#include <opencv/cv.h>
#include <time.h>

#include <QAction>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QTime>
#include <QToolBar>

#include "configurationengine.h"
#include "settings.h"

using namespace cv;

typedef struct _buffer_ {
        void   *start;
        size_t length;
} Buffer;


class CameraThread : public QThread
{
    Q_OBJECT

private:
    /// ToolBar Widgets
    QToolBar    *_threadToolBar;
    QAction     *_settingsAction, *_startAction, *_stopAction;

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

    /// Internal Frame Buffer
    QMutex       _mutex;
    QQueue<Mat > _cvMatbuffer;

    /// Frame Rate Calculator support data
    float _fps;
    QTime myTimer;

    /// Used to Open Capture Device and set V4l Conversion format
    void openCaptureDevice();

protected:
    int exec();
    void run();

private slots:
    void configure();

public:
    CameraThread(QObject *parent = 0);
    ~CameraThread();

    float getFPS();
    int getHeight();
    int getWidth();
    bool isConfigurated();

    QToolBar *toolBar();

    static QImage mat2qImage(Mat src);

public slots:
    Mat getFrame();
    void stop();

signals:
    void availableFrame();
    void configurated();
};

#endif // CAMERATHREAD_H
