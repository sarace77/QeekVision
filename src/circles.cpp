#include "circles.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <QDebug>

#include <opencv2/highgui/highgui.hpp>


Circles::Circles(QObject *parent) : ProcessThread(parent) {
    _circlesToolBar = new QToolBar();
    _circlesToolBar->setObjectName("Circles toolbar");

    _showCanny = new QCheckBox("Show Canny");

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
    _maxRadiusSlider->setValue(600);
    _maxRadiusSlider->setToolTip("Maximum radius to be detected. If unknown, put zero as default.");

    _errorLabel = new QLabel("Area shape Tolerance");
    _errorSlider = new QSpinBox();
    _errorSlider->setMinimum(1);
    _errorSlider->setMaximum(1280*1024);
    _errorSlider->setValue(200);
    _errorSlider->setToolTip("Maximum tolerance (in pixels) admitted for Shape Areas");

    _blurTypeLabel = new QLabel("Blur type:");
    _normalBlur = new QRadioButton("Standard Blur");
    _gaussianBlur = new QRadioButton("Gaussian Blur");
    _gaussianBlur->setChecked(true);
    connect(_gaussianBlur, SIGNAL(toggled(bool)), this, SLOT(selectBlurType(bool)));

    _kernelSizeLabel = new QLabel("Blur Kernel Size");
    _kernelSize = new QSlider(Qt::Horizontal);
    _kernelSize->setTickInterval(2);
    _kernelSize->setTickPosition(QSlider::TicksBelow);
    _kernelSize->setPageStep(2);
    _kernelSize->setMinimum(3);
    _kernelSize->setMaximum(15);
    _kernelSize->setValue(9);
    _kernelSize->setToolTip("It defines a (value x value) blur kernel");
    _kernelSizeValue = new QLineEdit("9");
    _kernelSizeValue->setAlignment(Qt::AlignRight);

    _kernelWidget = new QWidget();
    _kernelLayout = new QHBoxLayout();
    _kernelLayout->addWidget(_kernelSize, 3);
    _kernelLayout->addWidget(_kernelSizeValue, 1);
    _kernelWidget->setLayout(_kernelLayout);
    connect(_kernelSize, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));

    _sigmaLabel = new QLabel("Gaussian kernel standard deviation");
    _sigmaX = new QDoubleSpinBox();
    _sigmaX->setDecimals(2);
    _sigmaX->setValue(2);
    _sigmaX->setToolTip("Gaussian kernel standard deviation in X direction");
    _sigmaY = new QDoubleSpinBox();
    _sigmaY->setDecimals(2);
    _sigmaY->setValue(2);
    _sigmaY->setToolTip("Gaussian kernel standard deviation in X direction");
    _sigmaWidget = new QWidget();
    _sigmaLayout = new QHBoxLayout();
    _sigmaLayout->addWidget(_sigmaX);
    _sigmaLayout->addWidget(_sigmaY);
    _sigmaWidget->setLayout(_sigmaLayout);

    _countoursApproxLabel = new QLabel("Contours Approximation Type");
    _countoursApprox = new QComboBox();
    _countoursApprox->addItem("CV_CHAIN_APPROX_NONE");
    _countoursApprox->addItem("CV_CHAIN_APPROX_SIMPLE");
    _countoursApprox->addItem("CV_CHAIN_APPROX_TC89_L1");
    _countoursApprox->addItem("CV_CHAIN_APPROX_TC89_KCOS");

    _erodeDilateStepsLabel = new QLabel("Num of Erode/Dilate Steps");
    _erodeDilateSteps = new QSlider(Qt::Horizontal);
    _erodeDilateSteps->setMinimum(1);
    _erodeDilateSteps->setMaximum(8);
    _erodeDilateSteps->setValue(1);
    _erodeDilateSteps->setTickInterval(2);
    _erodeDilateSteps->setTickPosition(QSlider::TicksBelow);

    _circlesToolBar->addWidget(_showCanny);
    _circlesToolBar->addSeparator();
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
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_blurTypeLabel);
    _circlesToolBar->addWidget(_normalBlur);
    _circlesToolBar->addWidget(_gaussianBlur);
    _circlesToolBar->addWidget(_kernelSizeLabel);
    _circlesToolBar->addWidget(_kernelWidget);
    _circlesToolBar->addWidget(_sigmaLabel);
    _circlesToolBar->addWidget(_sigmaWidget);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_countoursApproxLabel);
    _circlesToolBar->addWidget(_countoursApprox);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_erodeDilateStepsLabel);
    _circlesToolBar->addWidget(_erodeDilateSteps);

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

Mat Circles::getCanny() {
    return _cannyFrame;
}

bool Circles::hasCanny() {
    return _showCanny->isChecked();
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
                int kSizeInt = _kernelSize->value();
                kSizeInt = (kSizeInt / 2) * 2 == kSizeInt ? kSizeInt - 1: kSizeInt;
                Size kSize = Size(kSizeInt, kSizeInt);
                if (_gaussianBlur->isChecked())
                    GaussianBlur(srcGray, srcGray, kSize, _sigmaX->value(), _sigmaY->value());
                else
                    blur(srcGray, srcGray, kSize);
                Canny(srcGray, _cannyFrame, _param2Slider->value(), _param1Slider->value());
                Mat element = getStructuringElement(MORPH_ELLIPSE, kSize, Point((int)kSizeInt/2, (int)kSizeInt/2));
                for(int i = 0; i < _erodeDilateSteps->value(); i++) {
                    dilate(_cannyFrame, srcGray, element);
                    erode(srcGray, _cannyFrame, element);
                }
                dilate(_cannyFrame, srcGray, element);
                _cannyFrame = srcGray;
                vector<vector<Point> > contours;
                vector<Vec4i> hierarchy;
                switch(_countoursApprox->currentIndex()) {
                case 0:
                    findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );
                    break;
                case 1:
                    findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                    break;
                case 2:
                    findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
                    break;
                case 3:
                    findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );
                    break;
                }

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

void Circles::sliderValueChanged(int value) {
    value = (value / 2) *2 == value ? value - 1: value;
    _kernelSizeValue->setText(QString("%1").arg(value));
}

void Circles::selectBlurType(bool value) {
    qDebug() << value;
    _sigmaLabel->setEnabled(value);
    _sigmaWidget->setEnabled(value);

}
