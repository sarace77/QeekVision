#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
#include "gigecamera.h"
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
#include "opencvcamera.h"
#include "v4lcamera.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
    capture3ad = new GigECamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_PV_API);
#else
    capture3ad = new V4LCamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_V4L);
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
    imageWidget = new QVDisplayWidget(ui->centralWidget);
    process3ad = new BGSubtractor();

    addToolBar(Qt::RightToolBarArea,process3ad->toolBar());
    process3ad->toolBar()->setVisible(false);

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(show()));
}

MainWindow::~MainWindow() {
    if (capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    if (process3ad->isRunning())
        process3ad->terminate();
    process3ad->deleteLater();
    delete imageWidget;
    delete ui;
}

void MainWindow::acceptedDriverSelection() {
    switch(driverSelectDialog->getDriverType()) {
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
    case DRIVER_PV_API:
        break;
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
    case DRIVER_V4L:
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
        delete capture3ad;
        capture3ad = new V4LCamera();
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
        break;
    default:
        delete capture3ad;
        capture3ad = new OpenCVCamera();
        break;
    }
    addToolBar(capture3ad->toolBar());
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(terminate()));
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));

}

void MainWindow::processFrame() {
    Mat src = capture3ad->getFrame();
    if (src.type() == CV_8UC1)
        cvtColor(src, src, CV_GRAY2RGB);
    process3ad->enqueue(src);
}


void MainWindow::showFrame() {
    if (!process3ad->toolBar()->isVisible())
        process3ad->toolBar()->setVisible(true);
    this->resize(capture3ad->getWidth() + 100, capture3ad->getHeight() + 63);
    imageWidget->displayImage(process3ad->dequeue());
}
