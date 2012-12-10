#include "gigeqeekdemowindow.h"
#include "ui_gigeqeekdemowindow.h"

#include <QDebug>

GigeQeekDemoWindow::GigeQeekDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GigeQeekDemoWindow)
{
    ui->setupUi(this);
    capture3ad = new GigECamera();
    addToolBar(capture3ad->toolBar());
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
}

GigeQeekDemoWindow::~GigeQeekDemoWindow()
{
    if (capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    delete ui;
}

void GigeQeekDemoWindow::showFrame() {
    resize(capture3ad->getWidth() + 20, capture3ad->getHeight() + 60);
    ui->label->setGeometry(10, 10, capture3ad->getWidth(), capture3ad->getHeight());
    ui->label->setText("");
    Mat src = capture3ad->getFrame();
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(src)));
}
