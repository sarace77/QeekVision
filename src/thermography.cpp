#include "thermography.h"

#include <QDebug>
#include "Defs.h"

Thermography::Thermography(QObject *parent) : QThread(parent) {
    _inBuffer.clear();
    _outBuffer.clear();
}

void Thermography::enqueue(Mat frm) {
    if(_inBuffMtx.tryLock(2000)) {
    _inBuffer.enqueue(frm);
    _inBuffMtx.unlock();
    } else {
        qWarning() << "[THERMOGRAPHY] - enqueue() - Unable to lock Mutex";
        return;
    }
}

Mat Thermography::dequeue() {
    if (_outBuffer.isEmpty()){
        qWarning() << "[THERMOGRAPHY] - dequeue() - Empty output Buffer!";
        return Mat(_STD_FRM_HGHT, _STD_FRM_WDTH, CV_8UC3, Scalar(0,0,0));
    }
    return _outBuffer.dequeue();
}

int Thermography::exec() {
    qDebug()<< "[THERMOGRAPHY] - exec() - Started";
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
                threshold(tmp, tmp, 127, 127, CV_THRESH_TOZERO_INV);
                tmp *= 4;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat bCh = tmp - bMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, 63, 255, CV_THRESH_TOZERO);
                threshold(tmp, tmp, 191, 255, CV_THRESH_TOZERO_INV);
                tmp *= 3;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat gCh = tmp -gMask;

                cvtColor(src, tmp, CV_RGB2GRAY);
                threshold(tmp, tmp, 127, 255, CV_THRESH_TOZERO);
                tmp *= 2;
                cvtColor(tmp, tmp, CV_GRAY2RGB);
                Mat rCh = tmp - rMask;

                Mat rgb = rCh + gCh + bCh;
                _outBuffer.enqueue(rgb);
                _inBuffMtx.unlock();
                emit availableThermography();
            } else {
                qWarning() << "[THERMOGRAPHY] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

void Thermography::run() {
    exec();
}

void Thermography::stop() {
    terminate();
    qDebug() << "[THERMOGRAPHY] - stop() - Terminated";
}
