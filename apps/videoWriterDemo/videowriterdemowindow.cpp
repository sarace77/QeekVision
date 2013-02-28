#include "videowriterdemowindow.h"
#include "ui_videowriterdemowindow.h"

#include "opencvcamera.h"
#include "v4lcamera.h"

videoWriterDemoWindow::videoWriterDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::videoWriterDemoWindow)
{
    once = true;
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_V4L);
    imageWidget = new QVDisplayWidget(ui->centralWidget);
    process3ad = new MPGWriter();

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(show()));

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();
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
    delete imageWidget;
}

void videoWriterDemoWindow::acceptedDriverSelection() {
    switch(driverSelectDialog->getDriverType()) {
    case DRIVER_V4L:
        break;
    default:
        delete capture3ad;
        capture3ad = new OpenCVCamera();
        break;
    }

    addToolBar(capture3ad->toolBar());
    addToolBar(Qt::RightToolBarArea, process3ad->toolBar());

    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(stop()));
    process3ad->toolBar()->setVisible(false);
}

void videoWriterDemoWindow::showFrame() {
    if (!process3ad->toolBar()->isVisible()) {
        process3ad->toolBar()->setVisible(true);
    }
    Mat src = capture3ad->getFrame();
    process3ad->enqueue(src);
    this->resize(capture3ad->getWidth() + process3ad->toolBar()->width(), capture3ad->getHeight() + 63);
    imageWidget->displayImage(src);
}

void videoWriterDemoWindow::_debugShow() {
    imshow("Test", process3ad->dequeue());
}
