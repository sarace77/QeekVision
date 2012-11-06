#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "v4lcamera.h"
#include "thermography.h"

#include <QDebug>

#include <opencv/highgui.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow) {
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    process3ad = new Thermography();
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    addToolBar(capture3ad->toolBar());
    process3ad->start();
    fDialog = new QFileDialog();
    fDialog->setDirectory(QString("//"));
    fDialog->setNameFilter("All Images files (*.bmp *.jpg *.png)");
    connect(fDialog, SIGNAL(accepted()), this, SLOT(openFile()));
    if(process3ad->hasToolBar()) {
        addToolBar(process3ad->toolBar());
        this->setMinimumWidth(capture3ad->toolBar()->width() + 80);
    } else {
        this->setMinimumWidth(capture3ad->toolBar()->width());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    if (capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
    process3ad->deleteLater();
}

void MainWindow::on_actionOpen_triggered() {
    fDialog->show();
}

void MainWindow::openFile() {
    if(fDialog->selectedFiles().at(0).isEmpty())
        return;
    process3ad->start();
    Mat frame = imread(fDialog->selectedFiles().at(0).toAscii().constData());
    process3ad->enqueue(frame.clone());
}

void MainWindow::processFrame() {
    process3ad->enqueue(capture3ad->getFrame().clone());
}

void MainWindow::showFrame() {
    Mat frame = process3ad->dequeue();
    while(frame.cols > 1600 || frame.rows > 1200) {
        pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
    }
    this->resize(frame.cols, frame.rows + 47 + capture3ad->toolBar()->size().height());
    ui->label->resize(frame.cols, frame.rows);
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(frame)));
}
