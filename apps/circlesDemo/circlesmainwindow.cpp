#include "circlesmainwindow.h"
#include "ui_circlesmainwindow.h"

#include "gigecamera.h"

#include <opencv2/imgproc/imgproc.hpp>

#include "ellipseobject.h"

#include <QDebug>

CirclesMainWindow::CirclesMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CirclesMainWindow)
{
    ui->setupUi(this);
    capture3ad = new GigECamera();
    process3ad = new Circles();
    addToolBar(capture3ad->toolBar());
    if(process3ad->hasToolBar()) {
        addToolBar(Qt::RightToolBarArea, process3ad->toolBar());
        process3ad->toolBar()->setVisible(false);
    }
    connect(capture3ad, SIGNAL(started()), process3ad, SLOT(start()));
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), process3ad, SLOT(stop()));
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

    blurDialog = new QDialog();
    blurLabel = new QLabel("");
    blurLayout = new QHBoxLayout();
    cannyDialog = new QDialog();
    cannyLabel = new QLabel("");
    cannyLayout = new QHBoxLayout();

    blurLayout->addWidget(blurLabel);
    blurDialog->setLayout(blurLayout);
    cannyLayout->addWidget(cannyLabel);
    cannyDialog->setLayout(cannyLayout);

    imageWidget = new QVDisplayWidget(ui->centralWidget);
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
    if (src.type() == CV_8UC1)
        cvtColor(src, src, CV_GRAY2RGB);
    process3ad->enqueue(src);
    this->resize(src.cols + process3ad->toolBar()->width(), src.rows < 640 ? 640 : src.rows);
}

void CirclesMainWindow::showFrame() {
    Mat frame = process3ad->dequeue();
    QString sFrameRate = QString("Capture FrameRate: %1, ").arg(capture3ad->getFPS());
    Point center, h1, h2, v1, v2;
    if (imageWidget->getMouseXPos() > 0 && imageWidget->getMouseYPos() > 0) {
        center = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos());
        h1 = Point(imageWidget->getMouseXPos() - 20, imageWidget->getMouseYPos());
        h2 = Point(imageWidget->getMouseXPos() + 20, imageWidget->getMouseYPos());
        v1 = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos() - 20);
        v2 = Point(imageWidget->getMouseXPos(), imageWidget->getMouseYPos() + 20);
    }  else {
        center = Point(frame.cols/2, frame.rows/2);
        h1 = Point(frame.cols/2 - 25, frame.rows/2);
        h2 = Point(frame.cols/2 + 25, frame.rows/2);
        v1 = Point(frame.cols/2, frame.rows/2 - 25);
        v2 = Point(frame.cols/2, frame.rows/2 + 25);
    }
    process3ad->setCenter(center);
    line (frame, h1, h2, Scalar(0,255,0), 2);
    line (frame, v1, v2, Scalar(0,255,0), 2);
    imageWidget->displayImage(frame);
    ui->circleDataBox->setGeometry(ui->label->width() - 220, ui->label->height() - 200, 220, 180);
    ui->ellipseBox->setGeometry(0,0, ui->label->width(), 120);
    EllipseObject _ellipse = process3ad->getEllipse();
    if (process3ad->ellipseFound()) {
        ellipseLabel->setText(QString("<H3><FONT COLOR=#FF0000>New Ellipse found!</FONT></H3>") + QString("degree: %1").arg((1.0 - _ellipse.getEccentricity())*360/6.28));
        ui->ellipseBox->setVisible(true);
    } else {
        ui->ellipseBox->setVisible(ui->ellipseBox->isVisible() && (dialogTimer.elapsed() < 250));
    }
    if (process3ad->circleFound()) {
        QString sCircleData = QString("<H1><FONT COLOR=#00FF00>Circle found!</FONT></H1>");
        imageWidget->displayText(_ellipse.getCenter().x, _ellipse.getCenter().y + _ellipse.getVRadius() + 5, sCircleData);
        ui->centerX->setText(QString("%1").arg((int) _ellipse.getCenter().x));
        ui->centerY->setText(QString("%1").arg((int) _ellipse.getCenter().y));
        ui->xRadius->setText(QString("%1").arg((int) _ellipse.getHRadius()));
        ui->yRadius->setText(QString("%1").arg((int) _ellipse.getVRadius()));
        ui->centerDistance->setText(QString("%1").arg(_ellipse.getDistanceFromPoint(center)));
        ui->circleDataBox->setVisible(true);
        ellipseLabel->setText(QString("<H1><FONT COLOR=#00FF00>Circle found!</FONT></H1>")+ QString("degree: %1").arg((1.0 - _ellipse.getEccentricity())*360/6.28));
        dialogTimer.start();
    } else {
        ui->circleDataBox->setVisible(ui->circleDataBox->isVisible() && (dialogTimer.elapsed() < 500));
    }

    if (process3ad->hasCannyFrame()) {
        Mat cannyFrame = process3ad->getCannyFrame();
        cvtColor(cannyFrame, cannyFrame, CV_GRAY2RGB);
        cannyLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(cannyFrame)));
        if (cannyDialog->isHidden())
            cannyDialog->show();
    } else {
        if (!cannyDialog->isHidden())
            cannyDialog->hide();
    }
    if (process3ad->hasBlurredFrame()) {
        Mat blurFrame = process3ad->getBlurredFrame();
        cvtColor(blurFrame, blurFrame, CV_GRAY2RGB);
        blurLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(blurFrame)));
        if (blurDialog->isHidden())
            blurDialog->show();
    } else {
        if (!blurDialog->isHidden())
            blurDialog->hide();
    }
    sFrameRate += QString("Processing FrameRate: %1").arg(process3ad->getFrameRate());
    ui->statusBar->showMessage(sFrameRate);
}
