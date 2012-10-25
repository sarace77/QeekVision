#include "videowriterdemowindow.h"
#include "ui_videowriterdemowindow.h"

#include "v4lcamera.h"

videoWriterDemoWindow::videoWriterDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::videoWriterDemoWindow)
{
    once = true;
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    process3ad = new MPGWriter();
    addToolBar(capture3ad->toolBar());
    if(process3ad->hasToolBar()) {
        addToolBar(process3ad->toolBar());
        this->setMinimumWidth(capture3ad->toolBar()->width() + process3ad->toolBar()->width());
    } else {
        this->setMinimumWidth(capture3ad->toolBar()->width());
    }
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(stop()));
}

videoWriterDemoWindow::~videoWriterDemoWindow()
{
    delete ui;
    if (capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    if (process3ad->isRunning())
        process3ad->terminate();
    process3ad->deleteLater();
}

void videoWriterDemoWindow::showFrame() {
    Mat src = capture3ad->getFrame();
    process3ad->enqueue(src);
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(capture3ad->getWidth(), capture3ad->getHeight());
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(src)));
}

void videoWriterDemoWindow::_debugShow() {
    imshow("Test", process3ad->dequeue());
}
