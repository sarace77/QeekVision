#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "v4lcamera.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    bg = NULL;
    rng = new RNG(12345);
    capture3ad = new V4LCamera();
    addToolBar(capture3ad->toolBar());
    connect(capture3ad, SIGNAL(availableFrame()), this, SLOT(processFrame()));
    connect(capture3ad, SIGNAL(terminated()), this, SLOT(initBG()));
    this->resize(120, 80);
    initBG();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initBG() {
    if (bg != NULL)
        delete bg;
    bg = new BackgroundSubtractorMOG2(3000, 32, true);
}

void MainWindow::processFrame() {
    srcFrame = capture3ad->getFrame();
    bg->operator ()(srcFrame, fgFrame);
    bg->getBackgroundImage(bgFrame);
    erode(fgFrame, fgFrame, Mat());
    dilate(fgFrame, fgFrame, Mat());

    vector<vector<Point > > contours;
    vector<Vec4i> hierarchy;
    findContours( fgFrame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<vector<Point > > contoursPoly(contours.size());
    for (int i = 0; i < contours.size(); i++)
        approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);

    QList<vector<Point > > contoursPolyList;
    for (int i = 0; i < contours.size(); i++) {
        contoursPolyList << contoursPoly.at(i);
    }

    for (int i = 0; i < contoursPolyList.size(); i++) {
        for (unsigned int j = 0; j < i; j++) {
            if ( compareBlobs(contoursPolyList.at(j), contoursPolyList.at(i)))
                contoursPolyList.removeAt(i--);
        }
    }
    vector<vector <Point > > bigContoursPoly;
    vector<Rect > blob(contoursPolyList.size());
    for (int i = 0; i < contoursPolyList.size(); i++) {
        bigContoursPoly.push_back(contoursPolyList.at(i));
        blob[i] = boundingRect(Mat(bigContoursPoly[i]));
    }
    for (int i = 0; i < contoursPolyList.size(); i++) {
        drawContours(srcFrame, bigContoursPoly, i, Scalar(255,0,0), 2);
        rectangle(srcFrame, blob[i], Scalar(rng->uniform(0, 255), rng->uniform(0, 255), rng->uniform(0, 255)), 2);
    }

    this->resize(capture3ad->getWidth(), capture3ad->getHeight() + 50 + capture3ad->toolBar()->size().height());
    ui->frameLabel->setGeometry(0, 0, capture3ad->getWidth(), capture3ad->getHeight());
    ui->frameLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(srcFrame)));
}

bool MainWindow::compareBlobs(vector<Point> firstBlob, vector<Point> secondBlob) {
    unsigned int count = 0;
    for (unsigned int i = 0; i < secondBlob.size(); i++) {
        if (pointPolygonTest(firstBlob, secondBlob.at(i), false) >= 0) {
            count++;
        }
    }
    return (count == secondBlob.size());
}
