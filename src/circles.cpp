#include "circles.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <QDebug>
Circles::Circles(QObject *parent) : ProcessThread(parent) {
    _circlesToolBar = new QToolBar();
    _circlesToolBar->setObjectName("Circles toolbar");

    _eccentricityThresholdLabel = new QLabel("Eccentricity Threshold");
    _eccentricityThreshold = new QDoubleSpinBox();
    _eccentricityThreshold->setDecimals(3);
    _eccentricityThreshold->setMinimum(0.001);
    _eccentricityThreshold->setMaximum(0.5);
    _eccentricityThreshold->setValue(0.01);
    _eccentricityThreshold->setSingleStep(0.001);
    _eccentricityThreshold->setToolTip("Max/Min Radius Percentage Ratio for considering ellipse an (approximated) circle");

    _param1Label = new QLabel("UpperThr");
    _param1Slider = new QSpinBox();
    _param1Slider->setMinimum(0);
    _param1Slider->setMaximum(255);
    _param1Slider->setValue(80);
    _param1Slider->setToolTip("Upper threshold for the internal Canny edge detector");

    _param2Label = new QLabel("Thresh");
    _param2Slider = new QSpinBox();
    _param2Slider->setMinimum(0);
    _param2Slider->setMaximum(255);
    _param2Slider->setValue(40);
    _param2Slider->setToolTip("Threshold for center detection.");

    _minRadiusLabel = new QLabel("MinRadius");
    _minRadiusSlider = new QSpinBox();
    _minRadiusSlider->setMinimum(0);
    _minRadiusSlider->setMaximum(2000);
    _minRadiusSlider->setValue(25);
    _minRadiusSlider->setToolTip("Minimum radius to be detected. If unknown, put zero as default.");

    _maxRadiusLabel = new QLabel("MaxRadius");
    _maxRadiusSlider = new QSpinBox();
    _maxRadiusSlider->setMinimum(0);
    _maxRadiusSlider->setMaximum(2000);
    _maxRadiusSlider->setValue(300);
    _maxRadiusSlider->setToolTip("Maximum radius to be detected. If unknown, put zero as default.");

    _errorLabel = new QLabel("Area shape Tolerance");
    _errorSlider = new QSpinBox();
    _errorSlider->setMinimum(1);
    _errorSlider->setMaximum(1280*1024);
    _errorSlider->setValue(200);
    _errorSlider->setToolTip("Maximum tolerance (in pixels) admitted for Shape Areas");


    _circlesToolBar->addWidget(_eccentricityThresholdLabel);
    _circlesToolBar->addWidget(_eccentricityThreshold);
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
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_errorLabel);
    _circlesToolBar->addWidget(_errorSlider);

    _ellipseFound = false;
}

Circles::~Circles() { }

bool Circles::circleFound() {
    return _circleFound;
}

bool Circles::ellipseFound() {
    return _ellipseFound;
}

EllipseObject Circles::getEllipse() {
    return _ellipse;
}

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
                _ellipseFound = false;
                _circleFound = false;
                Mat srcFrame = _inBuffer.dequeue();
                Mat srcGray;
                Mat dst = srcFrame;
                Point frameCenter(srcFrame.cols/2, srcFrame.rows/2);
                cvtColor(srcFrame, srcGray, CV_BGR2GRAY);
                GaussianBlur(srcGray, srcGray, Size(9,9), 2, 2);
                Canny(srcGray, srcGray, _param2Slider->value(), _param1Slider->value());
                dilate(srcGray, srcGray, Mat());
                vector<vector<Point> > contours;
                vector<Vec4i> hierarchy;
                findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                for (unsigned int i = 0; i < contours.size(); i++) {
                    double actual_area = fabs(contourArea(contours[i], false));
                    Rect rect = boundingRect(contours[i]);
                    if (actual_area < (_minRadiusSlider->value() * _minRadiusSlider->value() * M_PI)) {
                        continue;
                    }
                    if (_maxRadiusSlider->value() > _minRadiusSlider->value() &&
                            _maxRadiusSlider->value() < (rect.width/2 > rect.height/2 ? rect.width/2 : rect.height/2)) {
                        continue;
                    }
                    double estimated_area =  M_PI * (rect.width / 2) * (rect.height / 2);
                    double error = fabs(actual_area - estimated_area);
                    if (error > ((float) _errorSlider->value()))
                        continue;
                    _ellipseFound = true;
                    _ellipse.setCenter(Point(rect.x + rect.width/2, rect.y + rect.height/2));
                    _ellipse.setHRadius(rect.width/2);
                    _ellipse.setVRadius(rect.height/2);
                    float maxEcc = 1.0 + (float) _eccentricityThreshold->value();
                    float minEcc = 1.0 - (float) _eccentricityThreshold->value();
                    RotatedRect ellipseRect = fitEllipse(contours[i]);
                    if (_ellipse.getEccentricity() < minEcc || _ellipse.getEccentricity() > maxEcc) {
                        ellipse(dst, ellipseRect, Scalar(255,0,0), 2);
                    } else {
                        ellipse(dst, ellipseRect, Scalar(0,255,0), 2);
                        line(dst, frameCenter, _ellipse.getCenter(), Scalar(255,0,255), 1, CV_AA);
                        circle(dst, _ellipse.getCenter(), 3, Scalar(0,0,255), 2);
                        _circleFound = true;
                        break;
                    }
//                    circle(dst, _ellipse.getCenter(), _minRadiusSlider->value(), Scalar(255,255,255));
//                    circle(dst, _ellipse.getCenter(), _maxRadiusSlider->value(), Scalar(255,255,255));
                }
                _outBuffer.enqueue(dst);
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
