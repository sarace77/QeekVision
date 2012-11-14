#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <opencv/cv.h>

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QTime>
#include <QToolBar>

using namespace cv;
using namespace std;

class ProcessThread : public QThread
{
    Q_OBJECT

protected:
    QQueue<Mat > _inBuffer;
    QQueue<Mat > _outBuffer;
    QMutex _inBuffMtx;
    float _fps;
    QTime _fpsTimer;
public:
    ProcessThread(QObject *parent = 0);

    virtual bool hasToolBar() = 0;
    virtual QToolBar *toolBar() = 0;

    float getFrameRate();

public slots:
    Mat dequeue();
    void enqueue(Mat frm);

signals:
    void availableProcessedFrame();
};

#endif // PROCESSTHREAD_H
