#ifndef CIRCLES_H
#define CIRCLES_H

#include <opencv2/imgproc/imgproc.hpp>

#include "ellipseobject.h"
#include "processthread.h"
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

using namespace cv;
using namespace std;

class Circles : public ProcessThread
{
    Q_OBJECT
private:
    QDoubleSpinBox *_eccentricityThreshold;
    QSpinBox  *_param1Slider, *_param2Slider, *_minRadiusSlider, *_maxRadiusSlider, *_errorSlider;
    QLabel *_eccentricityThresholdLabel, *_param1Label, *_param2Label, *_minRadiusLabel, *_maxRadiusLabel, *_errorLabel;
    QToolBar *_circlesToolBar;
    EllipseObject _ellipse;
    bool _circleFound, _ellipseFound;

protected slots:
    int exec();
    void run();
    void stop();
public:
    Circles(QObject *parent = 0);
    ~Circles();

    bool hasToolBar();
    QToolBar *toolBar();
    bool circleFound();
    bool ellipseFound();
    EllipseObject getEllipse();
};

#endif // CIRCLES_H
