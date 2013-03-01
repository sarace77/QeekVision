#include "Defs.h"
#include "processthread.h"

ProcessThread::ProcessThread(QObject *parent) : QThread(parent) {
    _inBuffer.clear();
    _outBuffer.clear();
    _fps = 0.0;
}

float ProcessThread::getFrameRate() {
    return _fps;
}

void ProcessThread::enqueue(Mat frm) {
    if(_inBuffMtx.tryLock(2000)) {
        _inBuffer.enqueue(frm);
        _inBuffMtx.unlock();
#ifdef _DEBUG_PROCESS_THREADS
    } else {
        qWarning() << "[PROCESS_THREAD] - enqueue() - Unable to lock Mutex";
#endif //_DEBUG_PROCESS_THREADS
    }
}

Mat ProcessThread::dequeue() {
    if (_outBuffer.isEmpty()){
#ifdef _DEBUG_PROCESS_THREADS
        qWarning() << "[PROCESS_THREAD] - dequeue() - Empty output Buffer!";
#endif //_DEBUG_PROCESS_THREADS
        return Mat(_STD_FRM_HGHT, _STD_FRM_WDTH, CV_8UC3, Scalar(0,0,0));
    }
    return _outBuffer.dequeue();
}

