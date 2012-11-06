#ifndef MPGWRITER_H
#define MPGWRITER_H

#include <opencv/highgui.h>

#include <QAction>
#include <QTime>
#include <QToolBar>


#include "processthread.h"



using namespace cv;

class MPGWriter : public ProcessThread
{
      Q_OBJECT
private:
    QString _videoFileName;
    Size _frameSize;
    double _FPS, _adjustedFPS;
    unsigned int _count;
    QTime _timer;

    QToolBar *_mpgwriterToolBar;
    QAction *_startRecordAction, *_stopRecordAction;

    VideoWriter *_outStream;

    bool checkFrame(Mat frame);

private slots:
    void adjustFrameRate();

protected slots:
    int exec();
    void run();

public:
    MPGWriter(QString sFileName = QString());

    bool hasToolBar();
    QToolBar *toolBar();

public slots:
    void enqueue(Mat frm);
    void stop();

signals:
    void receivedFrame();
};

#endif // MPGWRITER_H
