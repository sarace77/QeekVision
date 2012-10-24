#include "Defs.h"
#include "thermography.h"

#include <QDebug>

Thermography::Thermography(QObject *parent) : ProcessThread(parent) {
    _thermographyToolBar = new QToolBar();
    _thermographyToolBar->setObjectName("Thermography toolbar");

    _redSlider = new QSlider(Qt::Horizontal);
    _redSlider->setMinimum(0);
    _redSlider->setMaximum(255);
    _redSlider->setValue(127);
    _redSlider->setToolTip("Red (Hot) Threshold limit");

    _greenHiSlider = new QSlider(Qt::Horizontal);
    _greenHiSlider->setMinimum(0);
    _greenHiSlider->setMaximum(255);
    _greenHiSlider->setValue(191);
    _greenHiSlider->setToolTip("Green (Mid_level) Threshold Low limit");

    _greenLoSlider = new QSlider(Qt::Horizontal);
    _greenLoSlider->setMinimum(0);
    _greenLoSlider->setMaximum(255);
    _greenLoSlider->setValue(63);
    _greenLoSlider->setToolTip("Green (Mid_level) Threshold High limit");

    _blueSlider = new QSlider(Qt::Horizontal);
    _blueSlider->setMinimum(0);
    _blueSlider->setMaximum(255);
    _blueSlider->setValue(127);
    _blueSlider->setToolTip("Blue (Cold) Threshold Low limit");

    _redSpin = new QSpinBox();
    _redSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _redSpin->setMinimum(0);
    _redSpin->setMaximum(255);
    _redSpin->setValue(127);
    _redSpin->setEnabled(false);

    _greenHiSpin = new QSpinBox();
    _greenHiSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _greenHiSpin->setMinimum(0);
    _greenHiSpin->setMaximum(255);
    _greenHiSpin->setValue(191);
    _greenHiSpin->setEnabled(false);

    _greenLoSpin = new QSpinBox();
    _greenLoSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _greenLoSpin->setMinimum(0);
    _greenLoSpin->setMaximum(255);
    _greenLoSpin->setValue(63);
    _greenLoSpin->setEnabled(false);

    _blueSpin = new QSpinBox();
    _blueSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _blueSpin->setMinimum(0);
    _blueSpin->setMaximum(255);
    _blueSpin->setValue(127);
    _blueSpin->setEnabled(false);

    _thermographyToolBar->addWidget(_redSlider);
    _thermographyToolBar->addWidget(_redSpin);
    _thermographyToolBar->addSeparator();
    _thermographyToolBar->addWidget(_greenLoSlider);
    _thermographyToolBar->addWidget(_greenLoSpin);
    _thermographyToolBar->addWidget(_greenHiSlider);
    _thermographyToolBar->addWidget(_greenHiSpin);
    _thermographyToolBar->addSeparator();
    _thermographyToolBar->addWidget(_blueSlider);
    _thermographyToolBar->addWidget(_blueSpin);
    _thermographyToolBar->setEnabled(false);

    connect(_redSlider, SIGNAL(valueChanged(int)), _redSpin, SLOT(setValue(int)));
    connect(_greenLoSlider, SIGNAL(valueChanged(int)), _greenLoSpin, SLOT(setValue(int)));
    connect(_greenHiSlider, SIGNAL(valueChanged(int)), _greenHiSpin, SLOT(setValue(int)));
    connect(_blueSlider, SIGNAL(valueChanged(int)), _blueSpin, SLOT(setValue(int)));
}

int Thermography::exec() {
    qDebug()<< "[THERMOGRAPHY] - exec() - Started";
    _thermographyToolBar->setEnabled(true);
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                Mat src = _inBuffer.dequeue(), tmp;
                Mat rMask(src.size(), CV_8UC3, Scalar(0,255,255));
                Mat gMask(src.size(), CV_8UC3, Scalar(255,0,255));
                Mat bMask(src.size(), CV_8UC3, Scalar(255,255,0));

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, _blueSlider->value(), 127, CV_THRESH_TOZERO_INV);
                tmp *= 4;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat bCh = tmp - bMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, _greenLoSlider->value(), 255, CV_THRESH_TOZERO);
                threshold(tmp, tmp, _greenHiSlider->value(), 255, CV_THRESH_TOZERO_INV);
                tmp *= 3;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat gCh = tmp -gMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, _redSlider->value(), 255, CV_THRESH_TOZERO);
                tmp *= 2;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat rCh = tmp - rMask;

                Mat rgb = rCh + gCh + bCh;
                _outBuffer.enqueue(rgb);
                _inBuffMtx.unlock();
                emit availableProcessedFrame();
            } else {
                qWarning() << "[THERMOGRAPHY] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

bool Thermography::hasToolBar() {
    return true;
}

void Thermography::run() {
    exec();
}

QToolBar *Thermography::toolBar() {
    return _thermographyToolBar;
}
