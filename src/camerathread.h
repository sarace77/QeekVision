#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <opencv/cv.h>

#include <QtCore>
#include <QtGui>

#include "Defs.h"

using namespace cv;

typedef struct _buffer_ {
        void   *start;
        size_t length;
} Buffer;

class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(QObject *parent = 0);
    ~CameraThread();

    float getFPS();
    QToolBar *toolBar();
    QMenu *menu();

    virtual int getHeight() = 0;
    virtual int getWidth() = 0;
    virtual bool isConfigurated() = 0;

    static QImage mat2qImage(Mat src);

public slots:
    Mat getFrame();
    virtual void stop() = 0;

signals:
    void availableFrame();
    void configurated();

protected:
    /// ToolBar Widgets
    QToolBar    *_threadToolBar;
    QAction     *_settingsAction, *_startAction, *_stopAction;
    QLabel      *_imageFormat;
    QRadioButton *_bgr, *_rgb;

    /// Menu
    QMenu *_cameraMenu;

    /// Internal Frame Buffer
    QMutex       _mutex;
    QQueue<Mat > _cvMatbuffer;

    /// Frame Rate Calculator support data
    float _fps;
    QTime myTimer;

protected slots:
    virtual void configure() = 0;
    void viewCameraNetInfo() { }
};

#endif // CAMERATHREAD_H
