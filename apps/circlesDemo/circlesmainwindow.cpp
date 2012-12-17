#include "circlesmainwindow.h"
#include "ui_circlesmainwindow.h"

#include "ellipseobject.h"

#include "gigecamera.h"
#include "opencvcamera.h"
#include "v4lcamera.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>

CirclesMainWindow::CirclesMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CirclesMainWindow)
{
    ui->setupUi(this);

    capture3ad = new GigECamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_PV_API);
    process3ad = new Circles();

    resize(500,60);

    blurWidget = new QVDisplayWidget();
    cannyWidget = new QVDisplayWidget();
    histogramWidget = new QVDisplayWidget();
    imageWidget = new QVDisplayWidget(ui->centralWidget);

    blurWidget->setWindowTitle("Blurred");
    cannyWidget->setWindowTitle("Canny");
    cannyWidget->setWindowTitle("Image Histogram");

    addToolBar(Qt::RightToolBarArea, process3ad->toolBar());
    process3ad->toolBar()->setVisible(false);

    histogramWidget->setParent(ui->centralWidget);

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(showMaximized()));
}

CirclesMainWindow::~CirclesMainWindow() {
    imageWidget->close();
    if (capture3ad->isRunning()) {
        capture3ad->stop();
    }
    delete ui;
    process3ad->deleteLater();
    capture3ad->deleteLater();
    delete driverSelectDialog;
    delete blurWidget;
    delete cannyWidget;
    delete histogramWidget;
    delete imageWidget;
    delete secondToolBar;
}

void CirclesMainWindow::acceptedDriverSelection() {
    switch(driverSelectDialog->getDriverType()) {
    case DRIVER_PV_API:
        break;
    case DRIVER_V4L:
        delete capture3ad;
        capture3ad = new V4LCamera();
        break;
    default:
        delete capture3ad;
        capture3ad = new OpenCVCamera();
        break;
    }
    addToolBar(capture3ad->toolBar());
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
}

void CirclesMainWindow::processFrame() {
    if(process3ad->hasToolBar())
        process3ad->toolBar()->setVisible(true);
    src = capture3ad->getFrame();
    if (src.type() == CV_8UC1)
        cvtColor(src, src, CV_GRAY2RGB);
    process3ad->enqueue(src);
    this->resize(src.cols + process3ad->toolBar()->width(), src.rows < 640 ? 640 : src.rows);
}

void CirclesMainWindow::showFrame() {
    Mat frame = process3ad->dequeue();
    QString sFrameRate = QString("Capture FrameRate: %1, ").arg(capture3ad->getFPS());
    Point center, h1, h2, v1, v2;
    if (imageWidget->getMouseXPos() > 0 && imageWidget->getMouseYPos() > 0) {
        center = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos());
        h1 = Point(imageWidget->getMouseXPos() - 20, imageWidget->getMouseYPos());
        h2 = Point(imageWidget->getMouseXPos() + 20, imageWidget->getMouseYPos());
        v1 = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos() - 20);
        v2 = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos() + 20);
    }  else {
        center = Point(frame.cols/2, frame.rows/2);
        h1 = Point(frame.cols/2 - 25, frame.rows/2);
        h2 = Point(frame.cols/2 + 25, frame.rows/2);
        v1 = Point(frame.cols/2, frame.rows/2 - 25);
        v2 = Point(frame.cols/2, frame.rows/2 + 25);
    }
    process3ad->setCenter(center);
    line (frame, h1, h2, Scalar(0,255,0), 2);
    line (frame, v1, v2, Scalar(0,255,0), 2);
    imageWidget->displayImage(frame);
    EllipseObject _ellipse = process3ad->getEllipse();
    float fEcc = ((float) _ellipse.getHRadius()) / ((float) _ellipse.getVRadius()) * 100;
    QString sText =  "";
    if (process3ad->ellipseFound()) {
        sText = QString("<H2><FONT COLOR=#FF0000>Circle found!</FONT></H2>");
        sText += QString("<FONT COLOR =#FF00FF><B>Eccentricity: </B>%1</FONT>").arg(100- fEcc);
    }
    if (process3ad->circleFound()) {
        sText = QString("<H2><FONT COLOR=#00FF00>Circle found!</FONT></H2>");
        sText += QString("<FONT COLOR =#FF00FF><B>Eccentricity: </B>%1<BR>").arg(100 - fEcc);
        sText += QString("<B>X-Distance: </B> %1<BR>").arg(center.x - _ellipse.getCenter().x);
        sText += QString("<B>Y-Distance: </B> %1</FONT>").arg(center.y - _ellipse.getCenter().y);
    }
    imageWidget->displayText(_ellipse.getCenter().x, _ellipse.getCenter().y + _ellipse.getVRadius() + 5, sText);

    if (process3ad->hasCannyFrame()) {
        Mat cannyFrame = process3ad->getCannyFrame();
        cvtColor(cannyFrame, cannyFrame, CV_GRAY2RGB);
        cannyWidget->displayImage(cannyFrame);
        if (cannyWidget->isHidden())
            cannyWidget->show();
    } else {
        if (!cannyWidget->isHidden())
            cannyWidget->hide();
    }
    if (process3ad->hasBlurredFrame()) {
        Mat blurFrame = process3ad->getBlurredFrame();
        cvtColor(blurFrame, blurFrame, CV_GRAY2RGB);
        blurWidget->displayImage(blurFrame);
        if (blurWidget->isHidden())
            blurWidget->show();
    } else {
        if (!blurWidget->isHidden())
            blurWidget->hide();
    }
    sFrameRate += QString("Processing FrameRate: %1").arg(process3ad->getFrameRate());
    ui->statusBar->showMessage(sFrameRate);
    Mat histFrame = process3ad->getHistogramPlot();
    pyrDown(histFrame, histFrame, Size(histFrame.cols/2, histFrame.rows/2));
    histogramWidget->displayImage(histFrame);
}
