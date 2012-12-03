#include "trackfollowerdemo.h"
#include "ui_trackfollowerdemo.h"

#include <QDebug>
#include <QDir>
#include <QTime>

#include <opencv2/highgui/highgui.hpp>

using namespace std;

TrackFollowerDemo::TrackFollowerDemo(QWidget *parent) : QMainWindow(parent), ui(new Ui::TrackFollowerDemo) {
    ui->setupUi(this);
    firstFrameAcquired = false;
    secondFrameAcquired = false;
    maskLoaded = false;
    capture3ad = new OpenCVCamera();
    maskDialog = new QFileDialog();
    select1stFrame = new QRadioButton("First Frame");
    select2ndFrame = new QRadioButton("Second Frame");
    maskDialog->setWindowTitle("Load Mask");
    select1stFrame->setChecked(true);
    select2ndFrame->setEnabled(firstFrameAcquired);
    ui->mainToolBar->addWidget(select1stFrame);
    ui->mainToolBar->addWidget(select2ndFrame);
    addToolBar(Qt::TopToolBarArea, capture3ad->toolBar());
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    connect(maskDialog, SIGNAL(accepted()), this, SLOT(loadInputMask()));
    ui->actionSaveFrame->setEnabled(false);

    optionToolBar = new QToolBar("Option Toolbar");
    blurLabel = new QLabel("Blur Kernel Size");
    blurLayout = new QHBoxLayout();
    blurSlider = new QSlider(Qt::Horizontal);
    blurSpinBox = new QSpinBox();
    blurWidget = new QWidget();
    canny1Label = new QLabel("Canny Threshold Value");
    canny1Layout = new QHBoxLayout();
    canny1Slider = new QSlider(Qt::Horizontal);
    canny1SpinBox = new QSpinBox();
    canny1Widget = new QWidget();
    canny2Label = new QLabel("Canny Threshold Max Value");
    canny2Layout = new QHBoxLayout();
    canny2Slider = new QSlider(Qt::Horizontal);
    canny2SpinBox = new QSpinBox();
    canny2Widget = new QWidget();
    countoursApproxLabel = new QLabel("Contours Approx Chain type");
    countoursApproxType = new QComboBox();
    threshLabel = new QLabel("Threshold Value");
    threshLayout = new QHBoxLayout();
    threshSlider = new QSlider(Qt::Horizontal);
    threshSpinBox = new QSpinBox();
    threshType = new QComboBox();
    threshTypeLabel = new QLabel("Threshold Type");
    threshWidget = new QWidget();

    addToolBar(Qt::RightToolBarArea, optionToolBar);

    blurLayout->addWidget(blurSlider);
    blurLayout->addWidget(blurSpinBox);
    blurSlider->setMinimum(1);
    blurSlider->setMaximum(59);
    blurSlider->setSingleStep(2);
    blurSlider->setValue(7);
    blurSlider->setTickPosition(QSlider::TicksBelow);
    blurSpinBox->setMinimum(1);
    blurSpinBox->setMaximum(59);
    blurSpinBox->setSingleStep(2);
    blurSpinBox->setValue(7);
    blurWidget->setLayout(blurLayout);
    canny1Layout->addWidget(canny1Slider);
    canny1Layout->addWidget(canny1SpinBox);
    canny1Widget->setLayout(canny1Layout);
    canny1Slider->setMaximum(255);
    canny1Slider->setMinimum(1);
    canny1Slider->setValue(40);
    canny1SpinBox->setMaximum(255);
    canny1SpinBox->setMinimum(1);
    canny1SpinBox->setValue(40);
    canny2Layout->addWidget(canny2Slider);
    canny2Layout->addWidget(canny2SpinBox);
    canny2Widget->setLayout(canny2Layout);
    canny2Slider->setMaximum(255);
    canny2Slider->setMinimum(1);
    canny2Slider->setValue(80);
    canny2SpinBox->setMaximum(255);
    canny2SpinBox->setMinimum(1);
    canny2SpinBox->setValue(80);
    countoursApproxType->addItem("CV_CHAIN_APPROX_NONE");
    countoursApproxType->addItem("CV_CHAIN_APPROX_SIMPLE");
    countoursApproxType->addItem("CV_CHAIN_APPROX_TC89_L1");
    countoursApproxType->addItem("CV_CHAIN_APPROX_TC89_KCOS");
    threshLayout->addWidget(threshSlider);
    threshLayout->addWidget(threshSpinBox);
    threshSlider->setMinimum(1);
    threshSlider->setMaximum(254);
    threshSlider->setSingleStep(1);
    threshSlider->setValue(20);
    threshSlider->setTickPosition(QSlider::TicksBelow);
    threshSlider->setTickInterval(16);
    threshSpinBox->setMinimum(1);
    threshSpinBox->setMaximum(254);
    threshSpinBox->setValue(20);
    threshType->addItem("CV_THRESH_BINARY");
    threshType->addItem("CV_THRESH_BINARY_INV");
    threshType->addItem("CV_THRESH_TRUNC");
    threshType->addItem("CV_THRESH_TOZERO");
    threshType->addItem("CV_THRESH_TOZERO_INV");
    threshWidget->setLayout(threshLayout);

    optionToolBar->addWidget(blurLabel);
    optionToolBar->addWidget(blurWidget);
    optionToolBar->addSeparator();
    optionToolBar->addWidget(threshLabel);
    optionToolBar->addWidget(threshWidget);
    optionToolBar->addWidget(threshTypeLabel);
    optionToolBar->addWidget(threshType);
    optionToolBar->addSeparator();
    optionToolBar->addWidget(canny1Label);
    optionToolBar->addWidget(canny1Widget);
    optionToolBar->addWidget(canny2Label);
    optionToolBar->addWidget(canny2Widget);
    optionToolBar->addSeparator();
    optionToolBar->addWidget(countoursApproxLabel);
    optionToolBar->addWidget(countoursApproxType);

    connect (blurSlider, SIGNAL(valueChanged(int)), blurSpinBox, SLOT(setValue(int)));
    connect (blurSpinBox, SIGNAL(valueChanged(int)), blurSlider, SLOT(setValue(int)));
    connect (threshSlider, SIGNAL(valueChanged(int)), threshSpinBox, SLOT(setValue(int)));
    connect (threshSpinBox, SIGNAL(valueChanged(int)), threshSlider, SLOT(setValue(int)));
    connect (canny1Slider, SIGNAL(valueChanged(int)), canny1SpinBox, SLOT(setValue(int)));
    connect (canny1SpinBox, SIGNAL(valueChanged(int)), canny1Slider, SLOT(setValue(int)));
    connect (canny2Slider, SIGNAL(valueChanged(int)), canny2SpinBox, SLOT(setValue(int)));
    connect (canny2SpinBox, SIGNAL(valueChanged(int)), canny2Slider, SLOT(setValue(int)));
    optionToolBar->setVisible(false);
}

TrackFollowerDemo::~TrackFollowerDemo()
{
    if(capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
    delete select1stFrame;
    delete select2ndFrame;
    delete ui;
}

void TrackFollowerDemo::loadInputMask() {
    if (maskDialog->selectedFiles().at(0).isEmpty()) {
        qWarning() << "[TRACK_FOLLOWER_DEMO] - loadInputMask() - No file selected!";
    } else {
        maskFrame = imread(maskDialog->selectedFiles().at(0).toAscii().constData());
        maskLoaded = true;
    }
}

void TrackFollowerDemo::on_actionLoadMask_triggered() {
    maskDialog->show();
}

void TrackFollowerDemo::on_actionSaveFrame_triggered() {
    if (imwrite("frame.png", firstFrame))
        ui->statusBar->showMessage("Frame succesfully saved!", 5000);
    else
        ui->statusBar->showMessage("Error saving frame!", 5000);
}

void TrackFollowerDemo::showFrame() {
    Mat canny, element, src;
    int thrType = CV_THRESH_BINARY;
    int approxMethod = CV_CHAIN_APPROX_NONE;
    QTime fpsTimer;
    vector<vector<Point> > countours;
    vector<Vec4i> hierarchy;

    ui->actionSaveFrame->setEnabled(secondFrameAcquired);
    fpsTimer.start();

    element = getStructuringElement(MORPH_DILATE, Size(blurSlider->value(), blurSlider->value()), Point(blurSlider->value()/2, blurSlider->value()/2));
    src = capture3ad->getFrame().clone();
    drawingFrame = Mat::zeros( src.size(), CV_8UC3 );

    if (!secondFrameAcquired) {
        setGeometry(80, 80, 640, maskLoaded ? 1023 : 543);
    } else {
        if (!optionToolBar->isVisible())
            optionToolBar->setVisible(true);
        setGeometry(80, 80, 1480, 1023);
    }
    if (select1stFrame->isChecked()) {
        firstFrameAcquired = true;
        firstFrame = src.clone();
        select2ndFrame->setEnabled(true);
        ui->firstFrameLabel->setGeometry(0, 0, 640, 480);
        QPixmap p = QPixmap::fromImage(CameraThread::mat2qImage(firstFrame));
        ui->firstFrameLabel->setPixmap(p.scaled(640, 480, Qt::KeepAspectRatio));
    } else {
        secondFrameAcquired = true;
        secondFrame = src.clone();
        ui->secondFrameLabel->setGeometry(640, 0, 640, 480);
        QPixmap p = QPixmap::fromImage(CameraThread::mat2qImage(secondFrame));
        ui->secondFrameLabel->setPixmap(p.scaled(640, 480, Qt::KeepAspectRatio));
        Mat srcGray = firstFrame.clone();
        blur(srcGray, srcGray, Size(blurSlider->value(), blurSlider->value()));
        switch(threshType->currentIndex()) {
        case 1:
            thrType = CV_THRESH_BINARY_INV;
            break;
        case 2:
            thrType = CV_THRESH_TRUNC;
            break;
        case 3:
            thrType = CV_THRESH_TOZERO;
            break;
        case 4:
            thrType = CV_THRESH_TOZERO_INV;
            break;
        default:
            break;
        }
        threshold(srcGray, srcGray, threshSlider->value(), 255, thrType);
        absdiff(srcGray, secondFrame, outputFrame);
        if (maskLoaded)
            outputFrame -= maskFrame;
        cvtColor(srcGray, canny, CV_RGB2GRAY);
        Canny(canny, canny, canny1Slider->value(), canny2Slider->value());
        dilate(canny, canny, element);
        erode(canny, canny, element);
        switch(countoursApproxType->currentIndex()) {
        case 1:
            approxMethod = CV_CHAIN_APPROX_SIMPLE;
            break;
        case 2:
            approxMethod = CV_CHAIN_APPROX_TC89_L1;
            break;
        case 3:
            approxMethod = CV_CHAIN_APPROX_TC89_KCOS;
            break;
        default:
            break;
        }

        findContours( canny, countours, hierarchy, CV_RETR_TREE, approxMethod, Point(0, 0) );
        for( unsigned int i = 0; i< countours.size(); i++ ) {
            Scalar color = Scalar( 255, 0, 0 );
            drawContours( drawingFrame, countours, i, color, 3, 8, hierarchy, 0, Point() );
            vector<Point > tmp = countours.at(0);
            const Point *elementPoints[1] = {&tmp[0]};
            int numberOfPoints = (int)tmp.size();
            fillPoly(drawingFrame, elementPoints, &numberOfPoints, 1, Scalar(0,0,255));
        }
        outputFrame = drawingFrame;
        ui->outputFrameLabel->setGeometry(640,480, 640, 480);
        QPixmap o = QPixmap::fromImage(CameraThread::mat2qImage(outputFrame));
        ui->outputFrameLabel->setPixmap(o.scaled(640, 480, Qt::KeepAspectRatio));
    }
    ui->maskedFrameLabel->setGeometry(0,480, 640, 480);
    if (maskLoaded) {
        QPixmap m = QPixmap::fromImage(CameraThread::mat2qImage(maskFrame));
        ui->maskedFrameLabel->setPixmap(m.scaled(640, 480, Qt::KeepAspectRatio));
    } else {
        ui->maskedFrameLabel->setText("<H1>No Input Mask Loaded</H1>");
        ui->maskedFrameLabel->setAlignment(Qt::AlignCenter);
    }
    ui->statusBar->showMessage(QString("%1").arg(1000.0/((float)fpsTimer.elapsed()+1)));
}
