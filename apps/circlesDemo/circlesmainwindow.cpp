#include "circlesmainwindow.h"
#include "ui_circlesmainwindow.h"

#include "opencvcamera.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "ellipseobject.h"

#include <QDebug>

CirclesMainWindow::CirclesMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CirclesMainWindow)
{
    ui->setupUi(this);
    capture3ad = new OpenCVCamera();
    process3ad = new Circles();
    addToolBar(capture3ad->toolBar());
    if(process3ad->hasToolBar()) {
        addToolBar(Qt::RightToolBarArea, process3ad->toolBar());
        process3ad->toolBar()->setVisible(false);
    }
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(terminate()));
    connect(process3ad, SIGNAL(availableProcessedFrame()), this, SLOT(showFrame()));
    ui->circleDataBox->setVisible(false);
    ui->ellipseBox->setVisible(false);
    ellipseLayout = new QHBoxLayout();
    ellipseLabel = new QLabel();
    ellipseLabel->setTextFormat(Qt::RichText);
    ellipseLayout->addWidget(ellipseLabel);
    ellipseLayout->setAlignment(Qt::AlignHCenter);
    ui->ellipseBox->setLayout(ellipseLayout);
    this->resize(500,60);
}

CirclesMainWindow::~CirclesMainWindow() {
    if (capture3ad->isRunning()) {
        capture3ad->stop();
    }
    delete ui;
    process3ad->deleteLater();
    capture3ad->deleteLater();
    delete ellipseLabel;
    delete ellipseLayout;
}

void CirclesMainWindow::processFrame() {
    if(process3ad->hasToolBar())
        process3ad->toolBar()->setVisible(true);
    src = capture3ad->getFrame();
    process3ad->enqueue(src);
    this->resize(src.cols + process3ad->toolBar()->width(), src.rows < 640 ? 640 : src.rows);
}

void CirclesMainWindow::showFrame() {
    Mat frame = process3ad->dequeue();
    Point center(frame.cols/2, frame.rows/2);
    Point v1(frame.cols/2, frame.rows/2 - 25);
    Point v2(frame.cols/2, frame.rows/2 + 25);
    Point h1(frame.cols/2 - 25, frame.rows/2);
    Point h2(frame.cols/2 + 25, frame.rows/2);
    circle(frame, center, 3, Scalar(255,0,255), 1);
    circle(frame, center, 6, Scalar(255,0,255), 1);
    circle(frame, center, 9, Scalar(255,0,255), 1);
    line (frame, v1, v2, Scalar(255,0,255), 1);
    line (frame, h1, h2, Scalar(255,0,255), 1);
    ui->statusBar->showMessage(tr("FPS: %1").arg(process3ad->getFrameRate()));
    ui->label->setGeometry(0, 0, frame.cols, frame.rows);
    ui->label->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(frame)));
    ui->circleDataBox->setGeometry(ui->label->width() - 220, ui->label->height() - 180, 220, 180);
    ui->ellipseBox->setGeometry(0,0, ui->label->width(), 80);
    EllipseObject _ellipse = process3ad->getEllipse();
    if (process3ad->ellipseFound()) {
        ellipseLabel->setText(QString("<H3><FONT COLOR=#FF0000>New Ellipse found!</FONT></H3>") + QString("degree: %1").arg((1.0 - _ellipse.getEccentricity())*360/6.28));
        ui->ellipseBox->setVisible(true);
    } else {
        ui->ellipseBox->setVisible(ui->ellipseBox->isVisible() && (dialogTimer.elapsed() < 250));
    }
    if (process3ad->circleFound()) {
        ui->centerX->setText(QString("%1").arg((int) _ellipse.getCenter().x));
        ui->centerY->setText(QString("%1").arg((int) _ellipse.getCenter().y));
        ui->xRadius->setText(QString("%1").arg((int) _ellipse.getHRadius()));
        ui->yRadius->setText(QString("%1").arg((int) _ellipse.getVRadius()));
        ui->centerDistance->setText(QString("%1").arg(_ellipse.getDistanceFromPoint(Point(frame.cols/2, frame.rows/2))));
        ui->circleDataBox->setVisible(true);
        ellipseLabel->setText(QString("<H1><FONT COLOR=#00FF00>Circle found!</FONT></H1>")+ QString("degree: %1").arg((1.0 - _ellipse.getEccentricity())*360/6.28));
        dialogTimer.start();
    } else {
        ui->circleDataBox->setVisible(ui->circleDataBox->isVisible() && (dialogTimer.elapsed() < 500));
    }

    if (process3ad->hasCannyImage())
        imshow("Canny", process3ad->getCanny());        
    if (process3ad->hasBlurImage())
        imshow("Canny", process3ad->getBlur());
}
