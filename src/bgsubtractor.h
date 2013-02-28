#ifndef BGSUBTRACTOR_H
#define BGSUBTRACTOR_H

#include <opencv2/opencv.hpp>

#include <QtGui>

#include "processthread.h"

using namespace cv;

class BGSubtractor : public ProcessThread
{
    Q_OBJECT

public:
    QToolBar *toolBar();

    BGSubtractor(QObject *parent = 0);
    ~BGSubtractor();

    bool hasToolBar();

private:
    Mat _srcFrame, _bgFrame, _fgFrame;
    BackgroundSubtractorMOG2 *_bg;
    RNG *_rng;

    QToolBar *_bgSubtractorToolBar;
    QSpinBox * _historySpinBox, *_thresholdSpinBox;

    bool compareBlobs(vector<Point > firstBlob, vector<Point > secondBlob);

private slots:
    int exec();
    void initBG();
    void run();
    void stop();

};

#endif // BGSUBTRACTOR_H
