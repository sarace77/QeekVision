#ifndef CIRCLES_H
#define CIRCLES_H

#include <opencv2/imgproc/imgproc.hpp>

#include "ellipseobject.h"
#include "processthread.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

using namespace cv;
using namespace std;

class Circles : public ProcessThread
{
    Q_OBJECT
private:
    QCheckBox *_showCanny;
    QDoubleSpinBox *_eccentricityThreshold, *_errorSlider, *_sigmaX, *_sigmaY;
    QSpinBox  *_param1Slider, *_param2Slider, *_minRadiusSlider, *_maxRadiusSlider;
    QLabel *_eccentricityThresholdLabel, *_param1Label, *_param2Label, *_minRadiusLabel, *_maxRadiusLabel, *_errorLabel, *_sigmaLabel;
    QRadioButton *_normalBlur, *_gaussianBlur;
    QSlider *_kernelSize, *_erodeDilateSteps;
    QLabel *_kernelSizeLabel, *_blurTypeLabel, *_countoursApproxLabel, *_erodeDilateStepsLabel;
    QLineEdit *_kernelSizeValue;
    QWidget *_kernelWidget, *_sigmaWidget;
    QHBoxLayout *_kernelLayout, *_sigmaLayout, *_imageFormatLayout;
    QComboBox *_countoursApprox;
    QToolBar *_circlesToolBar;
    EllipseObject _ellipse;
    bool _circleFound, _ellipseFound;

    Mat _cannyFrame;

private slots:
    void sliderValueChanged(int value);
    void selectBlurType(bool value);

protected slots:
    int exec();
    void run();
    void stop();

public:
    Circles(QObject *parent = 0);
    ~Circles();

    Mat getCanny();
    bool hasToolBar();
    bool hasCanny();
    QToolBar *toolBar();
    bool circleFound();
    bool ellipseFound();
    EllipseObject getEllipse();
};

#endif // CIRCLES_H
