#ifndef CIRCLES_H
#define CIRCLES_H

#include <opencv2/imgproc/imgproc.hpp>

#include "processthread.h"
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

using namespace cv;
using namespace std;

class Circles : public ProcessThread
{
    Q_OBJECT
private:
    QSpinBox *_radiusDistanceRatioSlider, *_param1Slider, *_param2Slider, *_minRadiusSlider, *_maxRadiusSlider;
    QLabel *_radiusDistanceRatioLabel, *_param1Label, *_param2Label, *_minRadiusLabel, *_maxRadiusLabel;
    //QSlider *_radiusDistanceRatioSlider, *_param1Slider, *_param2Slider, *_minRadiusSlider, *_maxRadiusSlider;
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
