#include "qeekdemowindow.h"
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
#include "gigecamera.h"
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
#include "opencvcamera.h"
#include "v4lcamera.h"
#include "ui_qeekdemowindow.h"

#define HEIGHT_OFFSET 47

QeekDemoWindow::QeekDemoWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QeekDemoWindow) {
    ui->setupUi(this);

#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
    capture3ad = new GigECamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_PV_API);
#else
    capture3ad = new V4LCamera();
    driverSelectDialog = new QVDriverSelect(DRIVER_V4L);
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
    imageWidget = new QVDisplayWidget(ui->centralwidget);

    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(acceptedDriverSelection()));
    connect(driverSelectDialog, SIGNAL(accepted()), this, SLOT(show()));

    driverSelectDialog->move(200, 200);
    driverSelectDialog->show();
    ui->actionSaveFrame->setEnabled(false);

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
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    addToolBar(capture3ad->toolBar());
}

void QeekDemoWindow::on_actionSaveFrame_triggered() {
    if (imwrite("capturedFrame.png", currentFrame))
        ui->statusbar->showMessage("Frame succesfully Saved!");
    else
        ui->statusbar->showMessage("Error saving frame!");
}

void QeekDemoWindow::showFrame() {
    ui->actionSaveFrame->setEnabled(true);
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + HEIGHT_OFFSET + capture3ad->toolBar()->size().height());
    currentFrame = capture3ad->getFrame();
    imageWidget->displayImage(currentFrame);
    if (imageWidget->hasMouseTracking()) {
        ui->statusbar->showMessage(QString("Pos: (%1,%2)").arg(imageWidget->getMouseXPos()).arg(imageWidget->getMouseYPos()));
    }
}
