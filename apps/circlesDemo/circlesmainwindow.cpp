#include "circlesmainwindow.h"
#include "ui_circlesmainwindow.h"

#include "v4lcamera.h"

#include <opencv2/imgproc/imgproc.hpp>

CirclesMainWindow::CirclesMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CirclesMainWindow)
{
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    process3ad = new Circles();
    addToolBar(capture3ad->toolBar());
    if(process3ad->hasToolBar()) {
        addToolBar(process3ad->toolBar());
/*        this->setMinimumWidth(capture3ad->toolBar()->width() + 80);
    } else {
        this->setMinimumWidth(capture3ad->toolBar()->width()); */
    }
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(terminate()));
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
}

CirclesMainWindow::~CirclesMainWindow() {
    if (capture3ad->isRunning()) {
        capture3ad->stop();
    }
    delete ui;
    process3ad->deleteLater();
    capture3ad->deleteLater();
}

void CirclesMainWindow::processFrame() {
    process3ad->enqueue(capture3ad->getFrame().clone());
}

void CirclesMainWindow::showFrame() {
    Mat frame = process3ad->dequeue();
    Point center(frame.cols/2, frame.rows/2);
    Point v1(frame.cols/2, frame.rows/2 - 25);
    Point v2(frame.cols/2, frame.rows/2 + 25);
    Point h1(frame.cols/2 - 25, frame.rows/2);
    Point h2(frame.cols/2 + 25, frame.rows/2);
    circle(frame, center, 4, Scalar(255,0,255), 1);
    circle(frame, center, 8, Scalar(255,0,255), 1);
    circle(frame, center, 12, Scalar(255,0,255), 1);
    line (frame, v1, v2, Scalar(255,0,255), 1);
    line (frame, h1, h2, Scalar(255,0,255), 1);
    while(frame.cols > 1600 || frame.rows > 1200) {
        pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
    }
    ui->statusBar->showMessage(tr("FPS: %1").arg(process3ad->getFrameRate()));
    //this->resize(frame.cols, frame.rows + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(frame.cols, frame.rows);
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(frame)));
}
