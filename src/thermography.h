#ifndef THERMOGRAPHY_H
#define THERMOGRAPHY_H

#include <opencv/cv.h>

#include <QtGui>

#include "processthread.h"

using namespace cv;

class Thermography : public ProcessThread
{
    Q_OBJECT

public:
    Thermography(QObject *parent = 0);

    bool hasToolBar();
    QToolBar *toolBar();

private:
    QToolBar *_thermographyToolBar;
    QSlider *_blueSlider;
    QSpinBox *_blueSpin;

private slots:
    int exec();
    void run();
    void stop();
};

#endif // THERMOGRAPHY_H
