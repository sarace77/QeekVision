#include "videowriterdemowindow.h"
#include "ui_videowriterdemowindow.h"

#include "v4lcamera.h"

videoWriterDemoWindow::videoWriterDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::videoWriterDemoWindow)
{
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    addToolBar(capture3ad->toolBar());
    process3ad = NULL;
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
    if (process3ad == NULL) {
        process3ad = new MPGWriter(capture3ad->getFPS(), Size(capture3ad->getWidth(), capture3ad->getHeight()));
        connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
        connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(_debugShow()));
        process3ad->start();
    }
    if (!process3ad->isRunning()) {
        delete process3ad;
        process3ad = new MPGWriter(capture3ad->getFPS(), Size(capture3ad->getWidth(), capture3ad->getHeight()));
        connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
        connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(_debugShow()));
        process3ad->start();
    }
    process3ad->enqueue(src);
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(capture3ad->getWidth(), capture3ad->getHeight());
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(src)));
}

void videoWriterDemoWindow::_debugShow() {
    imshow("Test", process3ad->dequeue());
}
