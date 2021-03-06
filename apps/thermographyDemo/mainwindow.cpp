#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencvcamera.h"
#include "v4lcamera.h"
#include "thermography.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),  ui(new Ui::MainWindow) {
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_V4L);

    imageWidget = new QVDisplayWidget(ui->centralWidget);
    process3ad = new Thermography();
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));

    process3ad->start();
    fDialog = new QFileDialog();
    fDialog->setDirectory(QString("//"));
    fDialog->setNameFilter("All Images files (*.bmp *.jpg *.png)");
    connect(fDialog, SIGNAL(accepted()), this, SLOT(openFile()));

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(show()));

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();
    process3ad->toolBar()->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
    process3ad->deleteLater();
    delete imageWidget;
}

void MainWindow::acceptedDriverSelection() {
    switch(driverSelectDialog->getDriverType()) {
    case DRIVER_V4L:
        break;
    default:
        delete capture3ad;
        capture3ad = new OpenCVCamera();
        break;
    }
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    addToolBar(capture3ad->toolBar());
    addToolBar(Qt::RightToolBarArea, process3ad->toolBar());
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
    if (!process3ad->toolBar()->isVisible())
        process3ad->toolBar()->setVisible(true);
    Mat frame = process3ad->dequeue();
    while(frame.cols > 1600 || frame.rows > 1200) {
        pyrDown(frame, frame, Size(frame.cols/2, frame.rows/2));
    }
    resize(frame.cols + process3ad->toolBar()->width(), frame.rows + 60);
    imageWidget->displayImage(frame);
}
