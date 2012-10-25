#ifndef MPGWRITER_H
#define MPGWRITER_H

#include <opencv/highgui.h>

#include "processthread.h"

using namespace cv;

class MPGWriter : public ProcessThread
{
      Q_OBJECT
private:
    QString _videoFileName;
    Size _frameSize;
    double _FPS;

protected slots:
    int exec();
    void run();
    void stop();

public:
    MPGWriter(double fps, Size fSize, QString sFileName = QString());

    bool hasToolBar();
    QToolBar *toolBar();
};

#endif // MPGWRITER_H
