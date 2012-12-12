#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "v4lcamera.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    capture3ad = new V4LCamera();
    imageWidget = new QVDisplayWidget(ui->centralWidget);
    addToolBar(capture3ad->toolBar());
    process3ad = new BGSubtractor();
    addToolBar(Qt::RightToolBarArea,process3ad->toolBar());
    process3ad->toolBar()->setVisible(false);
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(terminate()));
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
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


void MainWindow::processFrame() {
    process3ad->enqueue(capture3ad->getFrame());
}


void MainWindow::showFrame() {
    if (!process3ad->toolBar()->isVisible())
        process3ad->toolBar()->setVisible(true);
    this->resize(capture3ad->getWidth() + 100, capture3ad->getHeight() + 63);
    imageWidget->displayImage(process3ad->dequeue());
}
