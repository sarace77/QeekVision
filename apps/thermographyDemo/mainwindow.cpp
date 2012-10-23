#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow) {
    ui->setupUi(this);
    capture3ad = new CameraThread();
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    process3ad = new Thermography();
    connect(process3ad, SIGNAL(availableThermography()), this, SLOT(showFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    addToolBar(capture3ad->toolBar());
    process3ad->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
    process3ad->deleteLater();
}

void MainWindow::processFrame() {
    process3ad->enqueue(capture3ad->getFrame().clone());
}

void MainWindow::showFrame() {
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(capture3ad->getWidth(), capture3ad->getHeight());
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(process3ad->dequeue())));
}
