#ifndef TRACKFOLLOWERDEMO_H
#define TRACKFOLLOWERDEMO_H

#include <QFileDialog>
#include <QMainWindow>
#include <QRadioButton>

#include "opencvcamera.h"

namespace Ui {
class TrackFollowerDemo;
}

class TrackFollowerDemo : public QMainWindow
{
    Q_OBJECT
    
private:
    OpenCVCamera *capture3ad;

    QRadioButton *select1stFrame, *select2ndFrame;

    QFileDialog *maskDialog;

    Mat firstFrame, secondFrame, maskFrame;

    bool firstFrameAcquired, secondFrameAcquired;
    bool maskLoaded;

private slots:
    void on_actionLoadMask_triggered();
    void loadInputMask();
    void showFrame();

public:
    explicit TrackFollowerDemo(QWidget *parent = 0);
    ~TrackFollowerDemo();
    
private:
    Ui::TrackFollowerDemo *ui;
};

#endif // TRACKFOLLOWERDEMO_H
