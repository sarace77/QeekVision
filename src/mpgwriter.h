#ifndef MPGWRITER_H
#define MPGWRITER_H

#include <opencv/highgui.h>

#include <QAction>
#include <QToolBar>


#include "processthread.h"



using namespace cv;

class MPGWriter : public ProcessThread
{
      Q_OBJECT
private:
    QString _videoFileName;
    Size _frameSize;
    double _FPS;
    unsigned int _count;

    QToolBar *_mpgwriterToolBar;
    QAction *_startRecordAction, *_stopRecordAction;

    VideoWriter *_outStream;

    bool checkFrame(Mat frame);

protected slots:
    int exec();
    void run();

public:
    MPGWriter(QString sFileName = QString());

    bool hasToolBar();
    QToolBar *toolBar();

public slots:
    void stop();

};

#endif // MPGWRITER_H
