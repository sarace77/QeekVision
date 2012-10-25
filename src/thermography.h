#ifndef THERMOGRAPHY_H
#define THERMOGRAPHY_H

#include <opencv/cv.h>

#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QSlider>
#include <QSpinBox>

#include "processthread.h"

using namespace cv;

class Thermography : public ProcessThread
{
    Q_OBJECT

private:
    QToolBar *_thermographyToolBar;
    QSlider *_redSlider, *_greenHiSlider, *_greenLoSlider, *_blueSlider;
    QSpinBox *_redSpin, *_greenHiSpin, *_greenLoSpin, *_blueSpin;

protected slots:
    int exec();
    void run();
    void stop();

public:
    Thermography(QObject *parent = 0);

    bool hasToolBar();
    QToolBar *toolBar();

};

#endif // THERMOGRAPHY_H
