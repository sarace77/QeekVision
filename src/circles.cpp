#include "circles.h"

#include <QDebug>
Circles::Circles(QObject *parent) : ProcessThread(parent) {
    _circlesToolBar = new QToolBar();
    _circlesToolBar->setObjectName("Circles toolbar");

    _radiusDistanceRatioSlider = new QSlider();
    _radiusDistanceRatioSlider->setMinimum(2);
    _radiusDistanceRatioSlider->setMaximum(16);
    _radiusDistanceRatioSlider->setValue(8);
    _radiusDistanceRatioSlider->setToolTip("Radius Ratio");

    _param1Slider = new QSlider();
    _param1Slider->setMinimum(0);
    _param1Slider->setMaximum(255);
    _param1Slider->setValue(127);
    _param1Slider->setToolTip("Param1");

    _param2Slider = new QSlider();
    _param2Slider->setMinimum(0);
    _param2Slider->setMaximum(255);
    _param2Slider->setValue(63);
    _param2Slider->setToolTip("Param2");

    _minRadiusSlider = new QSlider();
    _minRadiusSlider->setMinimum(0);
    _minRadiusSlider->setMaximum(2000);
    _minRadiusSlider->setValue(60);
    _minRadiusSlider->setToolTip("min Radius");

    _maxRadiusSlider = new QSlider();
    _maxRadiusSlider->setMinimum(0);
    _maxRadiusSlider->setMaximum(2000);
    _maxRadiusSlider->setValue(0);
    _maxRadiusSlider->setToolTip("max Radius");

    _circlesToolBar->addWidget(_radiusDistanceRatioSlider);
    _circlesToolBar->addWidget(_param1Slider);
    _circlesToolBar->addWidget(_param2Slider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_minRadiusSlider);
    _circlesToolBar->addWidget(_maxRadiusSlider);
}

Circles::~Circles() { }

bool Circles::hasToolBar() {
    return true;
}

QToolBar *Circles::toolBar() {
    return _circlesToolBar;
}

int Circles::exec() {
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                vector<Vec3f > circles;
                Mat srcFrame = _inBuffer.dequeue();
                Mat srcGray;
                cvtColor(srcFrame, srcGray, CV_BGR2GRAY);
                GaussianBlur(srcGray, srcGray, Size(9,9), 2, 2);
                HoughCircles(srcGray, circles, CV_HOUGH_GRADIENT, 1, srcGray.rows/_radiusDistanceRatioSlider->value(),
                             _param1Slider->value(), _param2Slider->value(),
                             _minRadiusSlider->value(), _maxRadiusSlider->value());
                for (size_t i = 0; i < circles.size(); i++) {
                    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                    int radius = cvRound(circles[i][2]);
                    circle(srcFrame, center, 3, Scalar(0,255,0), -1, 8, 0);
                    circle(srcFrame, center, radius, Scalar(255,0,0), 3, 8, 0);
                    QString text=tr("(%1,%2)").arg(center.x).arg(center.y);
                    QString rText=tr("%1").arg(radius);
                    putText(srcFrame, text.toAscii().constData(), center, 3, 1, Scalar(0,255,0));
                    Point circum(cvRound(circles[i][0]), cvRound(circles[i][1]) + cvRound(circles[i][2]));
                    line(srcFrame, center, circum, Scalar(0,0,255), 3);
                    putText(srcFrame, rText.toAscii().constData(), circum, 3, 1, Scalar(0,0,255));
//                    Point a1(cvRound(circles[i][0]) - cvRound(circles[i][2]), cvRound(circles[i][1]) - cvRound(circles[i][2]));
//                    Point a2(cvRound(circles[i][0]) - cvRound(circles[i][2]), cvRound(circles[i][1]) + cvRound(circles[i][2]));
//                    Point a1(cvRound(circles[i][0]) + cvRound(circles[i][2]), cvRound(circles[i][1]) - cvRound(circles[i][2]));
//                    Point a1(cvRound(circles[i][0]) + cvRound(circles[i][2]), cvRound(circles[i][1]) + cvRound(circles[i][2]));
                    Rect square;
                    square.x = cvRound(circles[i][0]) - cvRound(circles[i][2]);
                    square.y = cvRound(circles[i][1]) - cvRound(circles[i][2]);
                    square += Size(2*radius, 2*radius);
                    rectangle(srcFrame, square, Scalar(255,0,255), 5);
                }
                _outBuffer.enqueue(srcFrame.clone());
                qDebug() << "Circles: " << circles.size();
                _inBuffMtx.unlock();
                emit availableProcessedFrame();
            } else {
                qWarning() << "[CIRCLES] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

void Circles::run() {
    exec();
}

void Circles::stop() {
    qDebug() << "[CIRCLES] - stop() - Stopping...";
    terminate();
}
