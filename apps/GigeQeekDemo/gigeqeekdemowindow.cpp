#include "gigeqeekdemowindow.h"
#include "ui_gigeqeekdemowindow.h"

#include <QDebug>

GigeQeekDemoWindow::GigeQeekDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GigeQeekDemoWindow)
{
    ui->setupUi(this);
    capture3ad = new GigECamera();
    displayWidget = new QVDisplayWidget(ui->centralWidget);
    addToolBar(capture3ad->toolBar());
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
}

GigeQeekDemoWindow::~GigeQeekDemoWindow()
{
    if (capture3ad->isRunning())
        capture3ad->stop();
    capture3ad->deleteLater();
    delete displayWidget;
    delete ui;
}

void GigeQeekDemoWindow::showFrame() {
    resize(capture3ad->getWidth() + 20, capture3ad->getHeight() + 60);
    displayWidget->displayImage(capture3ad->getFrame());
    if (displayWidget->isMouseTrackingEnabled()) {
        ui->statusBar->showMessage(QString("%1,%2").arg(displayWidget->getMouseXPos()).arg(displayWidget->getMouseYPos()), 500);
    }
}
