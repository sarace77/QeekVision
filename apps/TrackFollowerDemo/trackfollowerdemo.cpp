#include "trackfollowerdemo.h"
#include "ui_trackfollowerdemo.h"

#include <QDebug>
#include <QDir>

TrackFollowerDemo::TrackFollowerDemo(QWidget *parent) : QMainWindow(parent), ui(new Ui::TrackFollowerDemo) {
    ui->setupUi(this);
    firstFrameAcquired = false;
    secondFrameAcquired = false;
    maskLoaded = false;
    capture3ad = new OpenCVCamera();
    maskDialog = new QFileDialog();
    select1stFrame = new QRadioButton("First Frame");
    select2ndFrame = new QRadioButton("Second Frame");
    maskDialog->setWindowTitle("Load Mask");
    select1stFrame->setChecked(true);
    select2ndFrame->setEnabled(firstFrameAcquired);
    ui->mainToolBar->addWidget(select1stFrame);
    ui->mainToolBar->addWidget(select2ndFrame);
    addToolBar(Qt::TopToolBarArea, capture3ad->toolBar());
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(showFrame()));
    connect(maskDialog, SIGNAL(accepted()), this, SLOT(loadInputMask()));
}

TrackFollowerDemo::~TrackFollowerDemo()
{
    if(capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    delete select1stFrame;
    delete select2ndFrame;
    delete ui;
}

void TrackFollowerDemo::loadInputMask() {
    if (maskDialog->selectedFiles().at(0).isEmpty()) {
        qWarning() << "[TRACK_FOLLOWER_DEMO] - loadInputMask() - No file selected!";
    } else {

    }
}

void TrackFollowerDemo::on_actionLoadMask_triggered() {
    maskDialog->show();
}

void TrackFollowerDemo::showFrame() {
    Mat src = capture3ad->getFrame();
    if (!secondFrameAcquired) {
        resize(capture3ad->getWidth(), capture3ad->getHeight() + 63);
    } else {
        resize(2 * capture3ad->getWidth(), capture3ad->getHeight() + 63);
    }
    if (select1stFrame->isChecked()) {
        firstFrameAcquired = true;
        firstFrame = src.clone();
        select2ndFrame->setEnabled(true);
        ui->firstFrameLabel->setGeometry(0, 0, capture3ad->getWidth(), capture3ad->getHeight());
        ui->firstFrameLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(firstFrame)));
    } else {
        secondFrameAcquired = true;
        secondFrame = src.clone();
        if (maskLoaded)
            secondFrame -= maskFrame;
        ui->secondFrameLabel->setGeometry(capture3ad->getWidth(), 0, capture3ad->getWidth(), capture3ad->getHeight());
        ui->secondFrameLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(secondFrame)));
    }
}
