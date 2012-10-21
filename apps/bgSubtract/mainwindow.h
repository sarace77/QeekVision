#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camerathread.h"

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    CameraThread * capture3ad;

    Mat srcFrame, bgFrame, fgFrame;
    BackgroundSubtractorMOG2 *bg;
    RNG *rng;

private slots:
    void processFrame();
    void initBG();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    bool compareBlobs(vector<Point > firstBlob, vector<Point > secondBlob);
};

#endif // MAINWINDOW_H
