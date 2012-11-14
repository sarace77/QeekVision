#include "circles.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <QDebug>
Circles::Circles(QObject *parent) : ProcessThread(parent) {
    _circlesToolBar = new QToolBar();
    _circlesToolBar->setObjectName("Circles toolbar");

    _radiusDistanceRatioLabel = new QLabel("Eccentricity");
    //_radiusDistanceRatioSlider = new QSlider();
    _radiusDistanceRatioSlider = new QSpinBox();
    _radiusDistanceRatioSlider->setMinimum(1);
    _radiusDistanceRatioSlider->setMaximum(100);
    _radiusDistanceRatioSlider->setValue(10);
    _radiusDistanceRatioSlider->setToolTip("Max/Min Radius Percentage Ratio for considering ellipse an (approximated) circle");

    _param1Label = new QLabel("UpperThr");
    //_param1Slider = new QSlider();
    _param1Slider = new QSpinBox();
    _param1Slider->setMinimum(0);
    _param1Slider->setMaximum(255);
    _param1Slider->setValue(100);
    _param1Slider->setToolTip("Upper threshold for the internal Canny edge detector");

    _param2Label = new QLabel("Thresh");
    //_param2Slider = new QSlider();
    _param2Slider = new QSpinBox();
    _param2Slider->setMinimum(0);
    _param2Slider->setMaximum(255);
    _param2Slider->setValue(50);
    _param2Slider->setToolTip("Threshold for center detection.");

    _minRadiusLabel = new QLabel("MinRadius");
    //_minRadiusSlider = new QSlider();
    _minRadiusSlider = new QSpinBox();
    _minRadiusSlider->setMinimum(0);
    _minRadiusSlider->setMaximum(2000);
    _minRadiusSlider->setValue(20);
    _minRadiusSlider->setToolTip("Minimum radius to be detected. If unknown, put zero as default.");

    _maxRadiusLabel = new QLabel("MaxRadius");
    //_maxRadiusSlider = new QSlider();
    _maxRadiusSlider = new QSpinBox();
    _maxRadiusSlider->setMinimum(0);
    _maxRadiusSlider->setMaximum(2000);
    _maxRadiusSlider->setValue(0);
    _maxRadiusSlider->setToolTip("Maximum radius to be detected. If unknown, put zero as default.");

    _circlesToolBar->addWidget(_radiusDistanceRatioLabel);
    _circlesToolBar->addWidget(_radiusDistanceRatioSlider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_param1Label);
    _circlesToolBar->addWidget(_param1Slider);
    _circlesToolBar->addWidget(_param2Label);
    _circlesToolBar->addWidget(_param2Slider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_minRadiusLabel);
    _circlesToolBar->addWidget(_minRadiusSlider);
    _circlesToolBar->addWidget(_maxRadiusLabel);
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
                _fpsTimer.start();
                vector<Vec3f > circles;
                Mat srcFrame = _inBuffer.dequeue();
                Mat srcGray;
                Mat dst(srcFrame.size(), CV_8UC3);
                Point frameCenter(srcFrame.cols/2, srcFrame.rows/2);
                cvtColor(srcFrame, srcGray, CV_BGR2GRAY);
                GaussianBlur(srcGray, srcGray, Size(9,9), 2, 2);
                Canny(srcGray, srcGray, _param2Slider->value(), _param1Slider->value());
                dilate(srcGray, srcGray, Mat());
                vector<vector<Point> > contours;
                vector<Vec4i> hierarchy;
                findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                for (int i = 0; i < contours.size(); i++) {
                    double actual_area = fabs(contourArea(contours[i], false));
                    if (actual_area < (_minRadiusSlider->value() * _minRadiusSlider->value() * M_PI)) {
                        continue;
                    }

                    if (actual_area > (_maxRadiusSlider->value() * _maxRadiusSlider->value() * M_PI) &&
                            _maxRadiusSlider->value() > _minRadiusSlider->value()) {
                        continue;
                    }
                    RotatedRect ellipseRect = fitEllipse(contours[i]);
                    Rect rect = boundingRect(contours[i]);
                    Point ellipseCenter(rect.x + rect.width/2, rect.y + rect.height/2);
                    Point centerText = ellipseCenter + Point(rect.width/2 + 1, 0);
                    QString sCenter = QString("(%1, %2)").arg(ellipseCenter.x).arg(ellipseCenter.y);
                    double estimated_area =  M_PI * (rect.width / 2) * (rect.height / 2);
                    double error = fabs(actual_area - estimated_area);
                    if (error > 100.0)
                        continue;
                    float ecc = ((float)rect.width) / ((float) rect.height);
                    Point eccCenter(0, srcFrame.rows);
                    QString sEcc = QString("%1").arg(ecc);
                    float maxEcc = (float) _radiusDistanceRatioSlider->value();
                    maxEcc /= 100;
                    float minEcc = 1.0 - maxEcc;
                    maxEcc += 1.0;
                    Point distPoint(rect.x, rect.y);
                    float ccDistance = sqrt((ellipseCenter.x - frameCenter.x)*(ellipseCenter.x - frameCenter.x) + \
                                            (ellipseCenter.y - frameCenter.y)*(ellipseCenter.y - frameCenter.y));
                    QString sDist=tr("Dist: %1").arg(ccDistance);
                    if (ecc < minEcc || ecc > maxEcc) {
                        ellipse(srcFrame, ellipseRect, Scalar(255,0,0), 2);
                        putText(srcFrame, sEcc.toAscii().constData(), eccCenter, FONT_HERSHEY_SIMPLEX, 2, Scalar(255,0,0),2);
                    } else {
                        ellipse(srcFrame, ellipseRect, Scalar(0,0,255), 2);
                        putText(srcFrame, sEcc.toAscii().constData(), eccCenter, FONT_HERSHEY_SIMPLEX, 2, Scalar(0,0,255),2);
                    }
                    circle(srcFrame, ellipseCenter, 3, Scalar(0,255,0), 2);
                    putText(srcFrame, sCenter.toAscii().constData(), centerText, FONT_HERSHEY_SIMPLEX, 1, Scalar(0,255,0));
                    line(srcFrame, frameCenter, ellipseCenter, Scalar(255,0,255), 2);
                    putText(srcFrame, sDist.toAscii().constData(), distPoint, 3, 1, Scalar(255,0,255), 2);
                }
                _outBuffer.enqueue(srcFrame.clone());
                _inBuffMtx.unlock();
                emit availableProcessedFrame();
                _fps = 1000/_fpsTimer.elapsed();
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
