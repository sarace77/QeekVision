#include "Defs.h"
#include "processthread.h"

#include <QDebug>

ProcessThread::ProcessThread(QObject *parent) : QThread(parent) {
    _inBuffer.clear();
    _outBuffer.clear();
    _fps = 0.0;
    connect(&_watchdog, SIGNAL(timeout()), this, SLOT(watchdogTimeout()));
}

float ProcessThread::getFrameRate() {
    return _fps;
}

void ProcessThread::enqueue(Mat frm) {
//    if(_inBuffMtx.tryLock(2000)) {
        _inBuffer.enqueue(frm);
//        _inBuffMtx.unlock();
//    } else {
//        qWarning() << "[PROCESS_THREAD] - enqueue() - Unable to lock Mutex";
//    }
}

Mat ProcessThread::dequeue() {
    if (_outBuffer.isEmpty()){
        qWarning() << "[PROCESS_THREAD] - dequeue() - Empty output Buffer!";
        return Mat(_STD_FRM_HGHT, _STD_FRM_WDTH, CV_8UC3, Scalar(0,0,0));
    }
    return _outBuffer.dequeue();
}

void ProcessThread::watchdogTimeout() {
    qWarning() << "[PROCESS_THREAD] - watchdogTimeout() - Trying to restart locked service";
//    this->terminate();
    //_inBuffer.clear();
    _inBuffer.dequeue();
//    this->start();
//    _inBuffMtx.unlock();
}
