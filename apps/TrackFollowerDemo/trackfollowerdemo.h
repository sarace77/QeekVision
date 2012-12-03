#ifndef TRACKFOLLOWERDEMO_H
#define TRACKFOLLOWERDEMO_H

#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QToolBar>
#include <QWidget>

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

    Mat drawingFrame, firstFrame, maskFrame, outputFrame, secondFrame;

    bool firstFrameAcquired, secondFrameAcquired;
    bool maskLoaded;

    QComboBox *countoursApproxType, *threshType;
    QHBoxLayout *blurLayout, *canny1Layout, *canny2Layout, *threshLayout;
    QLabel *blurLabel, *canny1Label, *canny2Label, *countoursApproxLabel, *threshLabel, *threshTypeLabel;
    QSlider *blurSlider, *canny1Slider, *canny2Slider, *threshSlider;
    QSpinBox *blurSpinBox, *canny1SpinBox, *canny2SpinBox, *threshSpinBox;
    QToolBar *optionToolBar;
    QWidget *blurWidget, *canny1Widget, *canny2Widget, *threshWidget;

private slots:
    void loadInputMask();
    void on_actionLoadMask_triggered();
    void on_actionSaveFrame_triggered();
    void showFrame();

public:
    explicit TrackFollowerDemo(QWidget *parent = 0);
    ~TrackFollowerDemo();
    
private:
    Ui::TrackFollowerDemo *ui;
};

#endif // TRACKFOLLOWERDEMO_H
