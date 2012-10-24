#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "v4lcamera.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    addToolBar(capture3ad->toolBar());
    process3ad = new BGSubtractor();
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

MainWindow::~MainWindow() {
    if (capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    if (process3ad->isRunning())
        process3ad->terminate();
    process3ad->deleteLater();
    delete ui;
}


void MainWindow::processFrame() {
    process3ad->enqueue(capture3ad->getFrame());
}


void MainWindow::showFrame() {
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + 50 + capture3ad->toolBar()->size().height());
    ui->frameLabel->setGeometry(0, 0, capture3ad->getWidth(), capture3ad->getHeight());
    ui->frameLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(process3ad->dequeue())));
}
