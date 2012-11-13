#include "opencvcamera.h"

#include <QDebug>

OpenCVCamera::OpenCVCamera() : CameraThread() {
    _camera = new VideoCapture();
}

OpenCVCamera::~OpenCVCamera() {
    if (isRunning())
        terminate();
    delete _camera;
}

void OpenCVCamera::configure() {

}

int OpenCVCamera::exec() {
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - Started!";
    while(1) {
        myTimer.start();
        if (_mutex.tryLock(2000)) {
            static Mat img;
            _camera->read(img);
            _mutex.unlock();
            _fps = 1000.0/myTimer.elapsed();
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Unable to lock Mutex: ";
            }
    }
    return 0;
}

void OpenCVCamera::run() {
    if(_camera->open(_deviceName.toAscii().constData()))
        exec();
    else {
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open device: " << _deviceName;
        qDebug() << "CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Trying to open first device";
        if(_camera->open(0))
            exec();
        else
            qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open default Capture Device!";
    }
}

void OpenCVCamera::stop() {
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - Stopping...";
    _camera->release();
}


int OpenCVCamera::getHeight() {
    if (_camera->isOpened()) {

    }
    return _STD_FRM_HGHT;
}

int OpenCVCamera::getWidth() {
    if (_camera->isOpened()) {

    }
    return _STD_FRM_WDTH;
}
bool OpenCVCamera::isConfigurated() {
    return _camera->isOpened();
}

