#ifndef TRACKFOLLOWERDEMO_H
#define TRACKFOLLOWERDEMO_H

#include <QtCore>
#include <QtGui>

#include "camerathread.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"

namespace Ui {
class TrackFollowerDemo;
}

class TrackFollowerDemo : public QMainWindow
{
    Q_OBJECT
    
private:
    CameraThread *capture3ad;
    QVDisplayWidget *firstWidget, *secondWidget, *maskWidget, *outputWidget;
    QVDriverSelect *driverSelectDialog;

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
    void acceptedDriverSelection();
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
