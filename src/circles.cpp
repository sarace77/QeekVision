#include "circles.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <opencv2/highgui/highgui.hpp>

#include <QDebug>

Circles::Circles(QObject *parent) : ProcessThread(parent) {

    _blurTypeLabel = new QLabel("Blur type:");
    _circlesToolBar = new QToolBar();
    _countoursApprox = new QComboBox();
    _countoursApproxLabel = new QLabel("Contours Approximation Type");
    _eccentricityThreshold = new QDoubleSpinBox();
    _eccentricityThresholdLabel = new QLabel("Eccentricity Threshold");
    _erodeDilateSteps = new QSlider(Qt::Horizontal);
    _erodeDilateStepsLabel = new QLabel("Num of Erode/Dilate Steps");
    _errorLabel = new QLabel("Area shape Tolerance");
    _errorSlider = new QDoubleSpinBox();
    _gaussianBlur = new QRadioButton("Gaussian Blur");
    _kernelLayout = new QHBoxLayout();
    _kernelSize = new QSlider(Qt::Horizontal);
    _kernelSizeLabel = new QLabel("Blur Kernel Size");
    _kernelSizeValue = new QLineEdit();
    _kernelWidget = new QWidget();
    _maxRadiusLabel = new QLabel("MaxRadius");
    _maxRadiusSlider = new QSpinBox();
    _minRadiusLabel = new QLabel("MinRadius");
    _minRadiusSlider = new QSpinBox();
    _param1Label = new QLabel("UpperThr");
    _param1Slider = new QSpinBox();
    _param2Label = new QLabel("Thresh");
    _param2Slider = new QSpinBox();
    _showBlur = new QCheckBox("Show Blurred Image");
    _showCanny = new QCheckBox("Show Canny");
    _showLayout = new QHBoxLayout();
    _showWidget = new QWidget();
    _sigmaLabel = new QLabel("Gaussian kernel standard deviation");
    _sigmaX = new QDoubleSpinBox();
    _sigmaY = new QDoubleSpinBox();
    _sigmaLayout = new QHBoxLayout();
    _sigmaWidget = new QWidget();
    _standardBlur = new QRadioButton("Standard Blur");
    _thresholdLabel = new QLabel("Main Threshold value");
    _thresholdLayout = new QHBoxLayout();
    _thresholdSlider = new QSlider(Qt::Horizontal);
    _thresholdValue = new QLineEdit();
    _thresholdWidget = new QWidget();

    _circlesToolBar->setObjectName("Circles toolbar");

    _kernelLayout->addWidget(_kernelSize, 3);
    _kernelLayout->addWidget(_kernelSizeValue, 1);
    _showLayout->addWidget(_showBlur);
    _showLayout->addWidget(_showCanny);
    _sigmaLayout->addWidget(_sigmaX);
    _sigmaLayout->addWidget(_sigmaY);
    _thresholdLayout->addWidget(_thresholdSlider);
    _thresholdLayout->addWidget(_thresholdValue);

    _kernelWidget->setLayout(_kernelLayout);
    _showWidget->setLayout(_showLayout);
    _sigmaWidget->setLayout(_sigmaLayout);
    _thresholdWidget->setLayout(_thresholdLayout);

    _eccentricityThreshold->setToolTip("Max/Min Radius Percentage Ratio for considering ellipse an (approximated) circle");
    _errorSlider->setToolTip("Maximum tolerance (in pixels) admitted for Shape Areas");
    _kernelSize->setToolTip("It defines a (value x value) blur kernel");
    _maxRadiusSlider->setToolTip("Maximum radius to be detected. If unknown, put zero as default.");
    _minRadiusSlider->setToolTip("Minimum radius to be detected. If unknown, put zero as default.");
    _param1Slider->setToolTip("Upper threshold for the internal Canny edge detector");
    _param2Slider->setToolTip("Threshold for center detection.");
    _sigmaX->setToolTip("Gaussian kernel standard deviation in X direction");
    _sigmaY->setToolTip("Gaussian kernel standard deviation in X direction");

    _countoursApprox->addItem("CV_CHAIN_APPROX_NONE");
    _countoursApprox->addItem("CV_CHAIN_APPROX_SIMPLE");
    _countoursApprox->addItem("CV_CHAIN_APPROX_TC89_L1");
    _countoursApprox->addItem("CV_CHAIN_APPROX_TC89_KCOS");

    _eccentricityThreshold->setDecimals(3);
    _eccentricityThreshold->setMaximum(0.5);
    _eccentricityThreshold->setValue(0.01);
    _eccentricityThreshold->setSingleStep(0.001);

    _erodeDilateSteps->setMinimum(1);
    _erodeDilateSteps->setMaximum(8);
    _erodeDilateSteps->setValue(1);
    _erodeDilateSteps->setTickInterval(2);
    _erodeDilateSteps->setTickPosition(QSlider::TicksBelow);

    _errorSlider->setMinimum(1);
    _errorSlider->setMaximum(3840000);
    _errorSlider->setValue(3000);

    _kernelSize->setTickInterval(2);
    _kernelSize->setTickPosition(QSlider::TicksBelow);
    _kernelSize->setPageStep(2);
    _kernelSize->setMinimum(1);
    _kernelSize->setMaximum(65);
    _kernelSize->setValue(3);
    _kernelSizeValue->setText(QString("%1").arg(_kernelSize->value()));
    _kernelSizeValue->setAlignment(Qt::AlignRight);

    _maxRadiusSlider->setMinimum(100);
    _maxRadiusSlider->setMaximum(500);
    _maxRadiusSlider->setValue(250);

    _minRadiusSlider->setMinimum(1);
    _minRadiusSlider->setMaximum(500);
    _minRadiusSlider->setValue(70);

    _param1Slider->setMinimum(0);
    _param1Slider->setMaximum(255);
    _param1Slider->setValue(80);

    _param2Slider->setMinimum(0);
    _param2Slider->setMaximum(255);
    _param2Slider->setValue(40);

    _sigmaLabel->setEnabled(false);
    _sigmaX->setDecimals(2);
    _sigmaX->setValue(2);
    _sigmaX->setEnabled(false);
    _sigmaY->setDecimals(2);
    _sigmaY->setValue(2);
    _sigmaY->setEnabled(false);

    _standardBlur->setChecked(true);

    _thresholdSlider->setMinimum(1);
    _thresholdSlider->setMaximum(254);
    _thresholdSlider->setValue(95);
    _thresholdSlider->setTickPosition(QSlider::TicksBelow);
    _thresholdValue->setText(QString("%1").arg(_thresholdSlider->value()));

    _circlesToolBar->addWidget(_showWidget);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_blurTypeLabel);
    _circlesToolBar->addWidget(_standardBlur);
    _circlesToolBar->addWidget(_gaussianBlur);
    _circlesToolBar->addWidget(_kernelSizeLabel);
    _circlesToolBar->addWidget(_kernelWidget);
    _circlesToolBar->addWidget(_sigmaLabel);
    _circlesToolBar->addWidget(_sigmaWidget);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_thresholdLabel);
    _circlesToolBar->addWidget(_thresholdWidget);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_param1Label);
    _circlesToolBar->addWidget(_param1Slider);
    _circlesToolBar->addWidget(_param2Label);
    _circlesToolBar->addWidget(_param2Slider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_erodeDilateStepsLabel);
    _circlesToolBar->addWidget(_erodeDilateSteps);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_countoursApproxLabel);
    _circlesToolBar->addWidget(_countoursApprox);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_minRadiusLabel);
    _circlesToolBar->addWidget(_minRadiusSlider);
    _circlesToolBar->addWidget(_maxRadiusLabel);
    _circlesToolBar->addWidget(_maxRadiusSlider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_errorLabel);
    _circlesToolBar->addWidget(_errorSlider);
    _circlesToolBar->addSeparator();
    _circlesToolBar->addWidget(_eccentricityThresholdLabel);
    _circlesToolBar->addWidget(_eccentricityThreshold);

    circleFound_ = false;
    ellipseFound_ = false;

    frameCenter = Point(0, 0);

    connect(_gaussianBlur, SIGNAL(toggled(bool)), this, SLOT(selectBlurType(bool)));
    connect(_kernelSize, SIGNAL(valueChanged(int)), this, SLOT(kernelValueChanged(int)));
    connect(_thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdValueChanged(int)));
}

Circles::~Circles() {
    delete _blurTypeLabel;
    delete _circlesToolBar;
    delete _countoursApprox;
    delete _countoursApproxLabel;
    delete _eccentricityThreshold;
    delete _eccentricityThresholdLabel;
    delete _erodeDilateSteps;
    delete _erodeDilateStepsLabel;
    delete _errorLabel;
    delete _errorSlider;
    delete _gaussianBlur;
    delete _kernelLayout;
    delete _kernelSize;
    delete _kernelSizeLabel;
    delete _kernelSizeValue;
    delete _kernelWidget;
    delete _maxRadiusLabel;
    delete _maxRadiusSlider;
    delete _minRadiusLabel;
    delete _minRadiusSlider;
    delete _param1Label;
    delete _param1Slider;
    delete _param2Label;
    delete _param2Slider;
    delete _showBlur;
    delete _showCanny;
    delete _showLayout;
    delete _showWidget;
    delete _sigmaLabel;
    delete _sigmaX;
    delete _sigmaY;
    delete _sigmaLayout;
    delete _sigmaWidget;
    delete _standardBlur;
}

bool Circles::circleFound() {
    return circleFound_;
}

bool Circles::ellipseFound() {
    return ellipseFound_;
}

Mat Circles::getCannyFrame() {
    return cannyFrame_;
}

Mat Circles::getBlurredFrame() {
    return blurFrame_;
}

EllipseObject Circles::getEllipse() {
    return ellipse_;
}

bool Circles::hasBlurredFrame() {
    return _showBlur->isChecked();
}

bool Circles::hasCannyFrame() {
    return _showCanny->isChecked();
}

void Circles::setCenter(Point cPoint) {
    frameCenter = cPoint;
}

bool Circles::hasToolBar() {
    return true;
}

QToolBar *Circles::toolBar() {
    return _circlesToolBar;
}

int Circles::exec() {
    qDebug() << "[CIRCLES] - exec() - Started!";
    Mat srcFrame, srcGray, dst;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(50);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                _fpsTimer.start();
                ellipseFound_ = false;
                circleFound_ = false;
                if (!_inBuffer.isEmpty())
                srcFrame = _inBuffer.dequeue();
                if (!srcFrame.empty()) {
                    dst = srcFrame;
                    if (frameCenter.x == 0 && frameCenter.y == 0)
                        frameCenter = Point(srcFrame.cols/2, srcFrame.rows/2);
                    cvtColor(srcFrame, srcGray, CV_BGR2GRAY);
                    int kSizeInt = _kernelSize->value();
                    kSizeInt = (kSizeInt / 2) * 2 == kSizeInt ? kSizeInt - 1: kSizeInt;
                    Size kSize = Size(kSizeInt, kSizeInt);
                    if (_gaussianBlur->isChecked())
                        GaussianBlur(srcGray, srcGray, kSize, _sigmaX->value(), _sigmaY->value());
                    else
                        blur(srcGray, srcGray, kSize);
                    blurFrame_ = srcGray.clone();
                    threshold(srcGray, srcGray, _thresholdSlider->value(), 255, CV_THRESH_BINARY);
                    Canny(srcGray, srcGray, _param2Slider->value(), _param1Slider->value());
                    cannyFrame_ = srcGray.clone();
                    Mat element = getStructuringElement(MORPH_ELLIPSE, kSize, Point((int)kSizeInt/2, (int)kSizeInt/2));
                    for(int i = 0; i < _erodeDilateSteps->value(); i++) {
                        dilate(srcGray, srcGray, element);
                        erode(srcGray, srcGray, element);
                    }
                    dilate(srcGray, srcGray, element);
                    contours.clear();
                    hierarchy.clear();
                    switch(_countoursApprox->currentIndex()) {
                    case 1:
                        findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
                        break;
                    case 2:
                        findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
                        break;
                    case 3:
                        findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_KCOS, Point(0, 0) );
                        break;
                    case 0:
                    default:
                        findContours( srcGray, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, Point(0, 0) );
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
                        ellipseFound_ = true;
                        ellipse_.setCenter(Point(rect.x + rect.width/2, rect.y + rect.height/2));
                        ellipse_.setHRadius(rect.width/2);
                        ellipse_.setVRadius(rect.height/2);
                        float maxEcc = 1.0 + (float) _eccentricityThreshold->value();
                        float minEcc = 1.0 - (float) _eccentricityThreshold->value();
                        RotatedRect ellipseRect = fitEllipse(contours[i]);
                        if (ellipse_.getEccentricity() < minEcc || ellipse_.getEccentricity() > maxEcc) {
                            ellipse(dst, ellipseRect, Scalar(255,0,0), 2);
                        } else {
                            ellipse(dst, ellipseRect, Scalar(0,255,0), 2);
                            line(dst, frameCenter, ellipse_.getCenter(), Scalar(255,0,255), 1, CV_AA);
                            circle(dst, ellipse_.getCenter(), 3, Scalar(0,0,255), 2);
                            circleFound_ = true;
                            break;
                        }
                    }
                    _outBuffer.enqueue(dst);
                }
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
    qDebug() << "[CIRCLES] - run() - Starting...";
    exec();
}

void Circles::stop() {
    qDebug() << "[CIRCLES] - stop() - Stopping...";
    if(!_inBuffMtx.tryLock())
        qWarning() << "[CIRCLES] - stop() - Trying to release locked Mutex";
    _inBuffMtx.unlock();    
    terminate();
    msleep(1000);
    if (!isRunning())
        qDebug() << "[CIRCLES] - stop() - Stopped!";
    else
        qWarning() << "[CIRCLES] - stop() - Process still running";
}

void Circles::kernelValueChanged(int value) {
    value = (value / 2) *2 == value ? value - 1: value;
    _kernelSizeValue->setText(QString("%1").arg(value));
}

void Circles::selectBlurType(bool value) {
    _sigmaLabel->setEnabled(value);
    _sigmaX->setEnabled(value);
    _sigmaY->setEnabled(value);
}

void Circles::thresholdValueChanged(int value) {
    _thresholdValue->setText(QString("%1").arg(value));
}

