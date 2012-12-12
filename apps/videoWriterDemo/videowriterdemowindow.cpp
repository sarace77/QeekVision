#include "videowriterdemowindow.h"
#include "ui_videowriterdemowindow.h"

#include "v4lcamera.h"

videoWriterDemoWindow::videoWriterDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::videoWriterDemoWindow)
{
    once = true;
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    imageWidget = new QVDisplayWidget(ui->centralWidget);
    process3ad = new MPGWriter();

    addToolBar(capture3ad->toolBar());
    addToolBar(Qt::RightToolBarArea, process3ad->toolBar());

    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(stop()));

    process3ad->toolBar()->setVisible(false);
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
