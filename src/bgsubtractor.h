#ifndef BGSUBTRACTOR_H
#define BGSUBTRACTOR_H

#include <opencv2/opencv.hpp>

#include <QSpinBox>
#include <QToolBar>

#include "processthread.h"

using namespace cv;
using namespace std;

class BGSubtractor : public ProcessThread
{
    Q_OBJECT

private:
    Mat _srcFrame, _bgFrame, _fgFrame;
    BackgroundSubtractorMOG2 *_bg;
    RNG *_rng;

    QToolBar *_bgSubtractorToolBar;
    QSpinBox * _historySpinBox, *_thresholdSpinBox;

    bool compareBlobs(vector<Point > firstBlob, vector<Point > secondBlob);

protected slots:
    int exec();
    void initBG();
    void run();
    void stop();

public:
    BGSubtractor(QObject *parent = 0);
    ~BGSubtractor();

    bool hasToolBar();
    QToolBar *toolBar();
};

#endif // BGSUBTRACTOR_H
