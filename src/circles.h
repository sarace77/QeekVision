#ifndef CIRCLES_H
#define CIRCLES_H

#include <opencv2/imgproc/imgproc.hpp>

#include "processthread.h"
#include <QSlider>
#include <QSpinBox>

using namespace cv;
using namespace std;

class Circles : public ProcessThread
{
    Q_OBJECT
private:
    QSlider *_radiusDistanceRatioSlider, *_param1Slider, *_param2Slider, *_minRadiusSlider, *_maxRadiusSlider;
    //QSpinBox *_redSpin, *_greenHiSpin, *_greenLoSpin, *_blueSpin;
    QToolBar *_circlesToolBar;

protected slots:
    int exec();
    void run();
    void stop();

public:
    Circles(QObject *parent = 0);
    ~Circles();

    bool hasToolBar();
    QToolBar *toolBar();
};

#endif // CIRCLES_H
