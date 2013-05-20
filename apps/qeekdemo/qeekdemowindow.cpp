#include "qeekdemowindow.h"
#include "opencvcamera.h"
#include "v4lcamera.h"
#include "ui_qeekdemowindow.h"

#include <QGLWidget>

#define HEIGHT_OFFSET 47

QeekDemoWindow::QeekDemoWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QeekDemoWindow) {
    ui->setupUi(this);

    device = new CameraWrapper();

    capture3ad = device->selectedCamera;

//    imageWidget = new QVDisplayWidget(ui->centralwidget);
    imageWidget = new QVGLWidget(ui->centralwidget);

    ui->actionSaveFrame->setEnabled(false);
    ui->menubar->addMenu(device->cameraMenu());
    ui->menubar->addMenu(device->captureMenu());
    ui->menubar->addMenu(device->frameFormatMenu());
    addToolBar(device->toolbar());
    connect(device, SIGNAL(cameraTypeChanged()), this, SLOT(deviceChanged()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
}

QeekDemoWindow::~QeekDemoWindow() {
    delete ui;
    device->deleteLater();
    delete imageWidget;
}

void QeekDemoWindow::on_actionSaveFrame_triggered() {
    if (imwrite("capturedFrame.png", currentFrame))
        ui->statusbar->showMessage("Frame succesfully Saved!");
    else
        ui->statusbar->showMessage("Error saving frame!");
}

void QeekDemoWindow::showFrame() {
    ui->actionSaveFrame->setEnabled(true);
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + HEIGHT_OFFSET); // + capture3ad->toolBar()->size().height());
    currentFrame = capture3ad->getFrame();
    imageWidget->displayImage(currentFrame);
    if (imageWidget->hasMouseTracking()) {
        ui->statusbar->showMessage(QString("Pos: (%1,%2)").arg(imageWidget->getMouseXPos()).arg(imageWidget->getMouseYPos()));
    }
}

void QeekDemoWindow::deviceChanged() {
    ui->menubar->clear();
    ui->menubar->addMenu(ui->menu_File);
    ui->menubar->addMenu(device->cameraMenu());
    ui->menubar->addMenu(device->captureMenu());
    ui->menubar->addMenu(device->frameFormatMenu());
    disconnect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    capture3ad = device->selectedCamera;
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
}
