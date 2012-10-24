#include "qeekdemowindow.h"
#include "v4lcamera.h"
#include "ui_qeekdemowindow.h"

#define HEIGHT_OFFSET 47

QeekDemoWindow::QeekDemoWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QeekDemoWindow) {
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    addToolBar(capture3ad->toolBar());
}

QeekDemoWindow::~QeekDemoWindow() {
    delete ui;
    if (capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
}

void QeekDemoWindow::showFrame() {
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + HEIGHT_OFFSET + capture3ad->toolBar()->size().height());
    ui->label->resize(capture3ad->getWidth(), capture3ad->getHeight());
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(capture3ad->getFrame())));
}
