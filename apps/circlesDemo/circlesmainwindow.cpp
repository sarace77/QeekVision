#include "circlesmainwindow.h"
#include "ui_circlesmainwindow.h"

#include "v4lcamera.h"

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
        this->setMinimumWidth(capture3ad->toolBar()->width() + process3ad->toolBar()->width());
    } else {
        this->setMinimumWidth(capture3ad->toolBar()->width());
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
    while(frame.cols > 1600 || frame.rows > 1200) {
        pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
    }
    this->resize(frame.cols, frame.rows + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(frame.cols, frame.rows);
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(frame)));
}
