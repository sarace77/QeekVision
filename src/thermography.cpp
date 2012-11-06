#include "Defs.h"
#include "thermography.h"

#include <opencv/highgui.h>

#include <QDebug>

Thermography::Thermography(QObject *parent) : ProcessThread(parent) {
    _thermographyToolBar = new QToolBar();
    _thermographyToolBar->setObjectName("Thermography toolbar");

    _blueSlider = new QSlider(Qt::Horizontal);
    _blueSlider->setMinimum(0);
    _blueSlider->setMaximum(255);
    _blueSlider->setValue(127);
    _blueSlider->setToolTip("Blue (Cold) Threshold Low limit");
    _blueSlider->setInvertedAppearance(true);

    _blueSpin = new QSpinBox();
    _blueSpin->setGeometry(0,0,200,25);
    _blueSpin->setButtonSymbols(QAbstractSpinBox::NoButtons);
    _blueSpin->setMinimum(0);
    _blueSpin->setMaximum(255);
    _blueSpin->setValue(127);
    _blueSpin->setEnabled(false);

    _thermographyToolBar->addWidget(_blueSlider);
    _thermographyToolBar->addWidget(_blueSpin);
    _thermographyToolBar->setEnabled(false);

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
                Mat gray_inverted(src.size(), CV_8UC1, Scalar(255));

                cvtColor(src, tmp, CV_RGB2GRAY);
                absdiff(tmp, gray_inverted, tmp);
                threshold(tmp, tmp, 245, 255, CV_THRESH_TOZERO_INV);
                absdiff(tmp, gray_inverted, tmp);
                threshold(tmp, tmp, _blueSlider->value(), 255, CV_THRESH_TOZERO_INV);
                tmp *=3;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat bCh = tmp - bMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                unsigned int thrs_M = ((255 - _blueSlider->value()) / 2) + _blueSlider->value();
                unsigned int thrs_m = (_blueSlider->value() / 2) + 1;
                threshold(tmp, tmp, thrs_M, 255, CV_THRESH_TOZERO_INV);
                absdiff(tmp, gray_inverted, tmp);
                threshold(tmp, tmp, 255 - thrs_m, 255, CV_THRESH_TOZERO);
                absdiff(tmp, gray_inverted, tmp);
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat gCh = tmp -gMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, _blueSlider->value(), 255, CV_THRESH_TOZERO);
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
    qDebug() << "[THERMOGRAPHY] - run() - Starting";
    exec();
}

void Thermography::stop() {
    terminate();
    qDebug() << "[THERMOGRAPHY] - stop() - Terminated";
}

QToolBar *Thermography::toolBar() {
    return _thermographyToolBar;
}
