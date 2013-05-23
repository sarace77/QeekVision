#include "qeekdemowindow.h"
#include "opencvcamera.h"
#include "v4lcamera.h"
#include "ui_qeekdemowindow.h"

#include <QGLWidget>

#include <QTime>

#define HEIGHT_OFFSET 47

QeekDemoWindow::QeekDemoWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::QeekDemoWindow) {
    ui->setupUi(this);

    device = new CameraWrapper();

    capture3ad = device->selectedCamera;

    imageWidget_1 = new QVDisplayWidget(ui->centralwidget);
    imageWidget_2 = new QVGLWidget(ui->centralwidget);

    ui->actionSaveFrame->setEnabled(false);
    ui->menubar->addMenu(device->cameraMenu());
    ui->menubar->addMenu(device->captureMenu());
    ui->menubar->addMenu(device->frameFormatMenu());
    addToolBar(device->toolbar());

    widgetSelectorToolBar = new QToolBar("Widget Selector", this);
    graphicSceneButton = new QRadioButton("Standard Render", widgetSelectorToolBar);
    openGlButton = new QRadioButton("openGL Render", widgetSelectorToolBar);
    openGlButton->setChecked(true);
    graphicSceneButton->setChecked(false);
    widgetSelectorToolBar->addWidget(graphicSceneButton);
    widgetSelectorToolBar->addWidget(openGlButton);
    addToolBar(widgetSelectorToolBar);

    connect(device, SIGNAL(cameraTypeChanged()), this, SLOT(deviceChanged()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
}

QeekDemoWindow::~QeekDemoWindow() {
    delete ui;
    device->deleteLater();
    delete imageWidget_1;
    delete imageWidget_2;
    delete graphicSceneButton;
    delete openGlButton;
    delete widgetSelectorToolBar;
}

void QeekDemoWindow::on_actionSaveFrame_triggered() {
    if (imwrite("capturedFrame.png", currentFrame))
        ui->statusbar->showMessage("Frame succesfully Saved!");
    else
        ui->statusbar->showMessage("Error saving frame!");
}

void QeekDemoWindow::showFrame() {
    QTime timer;
    timer.start();
    imageWidget_1->setVisible(graphicSceneButton->isChecked());
    imageWidget_2->setVisible(openGlButton->isChecked());
    ui->actionSaveFrame->setEnabled(true);
    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + HEIGHT_OFFSET); // + capture3ad->toolBar()->size().height());
    currentFrame = capture3ad->getFrame();
    if (openGlButton->isChecked()) {
        imageWidget_2->displayImage(currentFrame);
        imageWidget_2->update();
    } else {
        imageWidget_1->displayImage(currentFrame);
        imageWidget_1->update();

    }
    if (imageWidget_1->hasMouseTracking() && graphicSceneButton->isChecked()) {
        ui->statusbar->showMessage(QString("Pos: (%1,%2)").arg(imageWidget_1->getMouseXPos()).arg(imageWidget_1->getMouseYPos()));
    } else {
        ui->statusbar->showMessage(QString("FPS: %1, render time: %2").arg(capture3ad->getFPS()).arg(timer.elapsed()));
    }
    if (imageWidget_2->hasMouseTracking() && openGlButton->isChecked()) {
        ui->statusbar->showMessage(QString("Pos: (%1,%2)").arg(imageWidget_2->getMouseXPos()).arg(imageWidget_2->getMouseYPos()));
    } else {
        ui->statusbar->showMessage(QString("FPS: %1, render time: %2").arg(capture3ad->getFPS()).arg(timer.elapsed()));
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
