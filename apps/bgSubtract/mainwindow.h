#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camerathread.h"
#include "bgsubtractor.h"
#include "qvdisplaywidget.h"

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
    CameraThread *capture3ad;
    BGSubtractor *process3ad;
    QVDisplayWidget *imageWidget;

private slots:
    void processFrame();
    void showFrame();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
};

#endif // MAINWINDOW_H
