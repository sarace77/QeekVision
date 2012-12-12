#include "qeekdemowindow.h"
#include "gigecamera.h"
#include "opencvcamera.h"
#include "v4lcamera.h"
#include "ui_qeekdemowindow.h"

#define HEIGHT_OFFSET 47

QeekDemoWindow::QeekDemoWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QeekDemoWindow) {
    ui->setupUi(this);

    capture3ad = new GigECamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_PV_API);
    imageWidget = new QVDisplayWidget(ui->centralwidget);

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(show()));

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();

}

QeekDemoWindow::~QeekDemoWindow() {
    delete ui;
    if (capture3ad) {
        if (capture3ad->isRunning())
            capture3ad->stop();
        capture3ad->deleteLater();
    }
    delete imageWidget;
}

void QeekDemoWindow::acceptedDriverSelection() {
    switch(driverSelectDialog->getDriverType()) {
    case DRIVER_PV_API:
        break;
    case DRIVER_V4L:
        delete capture3ad;
        capture3ad = new V4LCamera();
        break;
    default:
        delete capture3ad;
        capture3ad = new OpenCVCamera();
        break;
    }
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    addToolBar(capture3ad->toolBar());
}

void QeekDemoWindow::showFrame() {
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + HEIGHT_OFFSET + capture3ad->toolBar()->size().height());
    imageWidget->displayImage(capture3ad->getFrame());
    if (imageWidget->hasMouseTracking()) {
        ui->statusbar->showMessage(QString("Pos: (%1,%2)").arg(imageWidget->getMouseXPos()).arg(imageWidget->getMouseYPos()));
    }
}
