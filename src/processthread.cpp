#include "Defs.h"
#include "processthread.h"

#include <QDebug>

ProcessThread::ProcessThread(QObject *parent) : QThread(parent) {
    _inBuffer.clear();
    _outBuffer.clear();
}

void ProcessThread::enqueue(Mat frm) {
    if(_inBuffMtx.tryLock(2000)) {
    _inBuffer.enqueue(frm);
    _inBuffMtx.unlock();
    } else {
        qWarning() << "[PROCESS_THREAD] - enqueue() - Unable to lock Mutex";
        return;
    }
}

Mat ProcessThread::dequeue() {
    if (_outBuffer.isEmpty()){
        qWarning() << "[PROCESS_THREAD] - dequeue() - Empty output Buffer!";
        return Mat(_STD_FRM_HGHT, _STD_FRM_WDTH, CV_8UC3, Scalar(0,0,0));
    }
    return _outBuffer.dequeue();
}

