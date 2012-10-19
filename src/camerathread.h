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
    QToolBar    *_threadToolBar;
    QAction     *_settingsAction, *_startAction, *_stopAction;

private:
    ConfigurationEngine *_configEngine;

    QString     _deviceName;

    struct v4l2_buffer              _buf;
    Buffer                          *_buffers;
    struct v4lconvert_data          *_v4lconvert_data;
    int                             _fd, _r;
    fd_set                          _fds;
    struct v4l2_format              _fmt;
    unsigned int                    _n_buffers;
    struct v4l2_requestbuffers      _req;
    struct v4l2_format              _src_fmt;
    struct timeval                  _tv;
    enum v4l2_buf_type              _type;

    QMutex       _mutex;
    QQueue<Mat > _cvMatbuffer;

    float _fps;

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
